---
title: "Parallel Paradigms and Parallel Algorithms"
teaching: 20
exercises: 10
questions:
- "How do I split the work?"
objectives:
- "Introduce Message Passing and Data Parallel"
- "Introduce Parallel Algorithm"
- "Introduce standard communication and data storage patterns"
keypoints:
- "Two major paradigms, message passing and data parallel."
- "MPI implements the Message Passing paradigm"
- "Several standard patterns: Trivial, Queue, Master / Worker, Domain Decomposition, All-to-All"
---

## Parallel Paradigms

How to realize a parallel computing is roughly divided into two camps: one is "data parallel" and the other is "message passing". MPI (Message Passing Interface, the parallelization method we use in our lessons) obviously belongs to the second camp. "openMP" belongs to the first. In message passing paradigm, each CPU (or a core) runs an independent program. Parallelism is achieved by receiving data which it doesn't have and sending data which it has. In data parallel paradigm, there are many different data and the same operations (instructions in assembly language speaking) are performed on these data at the same time. Parallelism is achieved by how many different data a single operation can act on. This division is mainly due to historical development of parallel architectures: one follows from shared memory architecture like SMP (Shared Memory Processor) and the other from distributed computer architecture. A familiar example of the shared memory architecture is GPU (or multi-core CPU) architecture and a familiar example of the distributed computing architecture is cluster computer. Which architecture is more useful depends on what kind of problems you have. Sometimes, one has to use both!

Consider a simple loop which can be sped up if we have many CPU's (or cores) for the illustration.

do i=1,N

  a(i) = b(i) + c(i)
  
enddo

in Fortran, and

for(i=0;i<N;i++) {

  a[i] = b[i] + c[i]
    
}

in C. If we have N CPU's (or cores), each element of the loop can be computed in just one step (factor N speed-up).

### Data Parallel

A kind of standard method for programming in data parallel fashion is "openMP". To understand what data parallel means, let's consider the following bit of openMP code which parallelize the above loop.

!$omp parallel do

do i=1,N

  a(i) = b(i) + c(i)
  
enddo

in Fortran, and

#pragma omp parallel for

for(i=0;i<N;i++) {

  a[i] = b[i] + c[i]
    
}

in C.

In both languages, parallelization is achieved by just one additional line which is handled by preprocessor (!$ in Fortran and # in C) in the compile stage. This is possible since the computer system architecture supports "openMP" and all the complicated mechanism for parallelization is hidden. Actually, this means that the system architecture has a shared memory view of variables and each CPU (or core) can access to all the memory address. So, the compiler "calculates" the address off-set for each CPU (or core) and let each CPU (or core) compute on a part of the whole data. Here, the catch word is shared memory which allows all CPU's (or cores) to access all the address space.

### Message Passing

In the message passing paradigm, each processor runs its own program and works on its own data.
To work on the same problem in parallel, they communicate by sending messages to each other. Again using the above example, each CPU (or core) runs just

do i=1,m

  a(i) = b(i) + c(i)
  
enddo

in Fortran, and

for(i=0;i<m;i++) {

  a[i] = b[i] + c[i];

}

in C.

Other than changing the number of loops from N to m, the code is exactly the same. But the parallelization by message passing is not complete yet. In message passing paradigm, each CPU (or core) is independent from the other CPU's (or cores). We must make sure that each CPU (or core) has correct data to compute and writes out the result in correct order. This part depends on the computer system. Let's assume that the system is a cluster computer. In a cluster computer, sometimes only one CPU (or core) has an access to the file system. In this case, this particular CPU reads in the whole data and sends the correct data to each CPU (or core) (including itself). MPI communications! After the computation, each CPU (or core) send the result to that particular CPU (or core). This particular CPU writes out the received data in a file in correct order. If the cluster computer supports a parallel file system, each CPU (or core) reads the correct data from one file, computes and writes out the result to one file in correct order.

Both data parallel and message passing achieves the following, logically.

![Each rank has it's own data]({{ page.root }}{% link files/dataparallel.png %})



## Algorithm Design


### Queue

A task queue is a simple implementation of "Embarassingly Parallel (EP)" problem.
Each worker will get tasks from a predefined queue.
The tasks can be very different and take different amounts of time,
but when a worker has completed its tasks, it will pick the next one
from the queue.

![Each rank taking one task from the top of a queue]({{ page.root }}{% link files/queue.png %})

In an MPI code,
the queue approach requires the ranks to communicate what they are doing to
all the other ranks, resulting in some communication overhead.
If several ranks are using the same data, communicating it can also create
some overhead.

### Master / Worker

The master / worker approach is a more flexible version of the queue method.
We hire a manager to distribute tasks to the workers.
The master can run some complicated logic to decide wich tasks to give to a
worker.
The master can also perform any serial parts of the program.

![A master rank controlling the queue]({{ page.root }}{% link files/master.png %})

In an MPI implementation, main function will usually contain an "if"
statement that determines whether the rank is the master or a worker.
The master usually executes a completely different code from the workers.

Naturally the Master / Worker approach reserves a rank to be the master.
In a large application this is usually a small cost.
The larger cost is a bit more subtle. Because every node needs to communicate with
the master, the bandwidth of the master rank can become bottleneck.

### Pipeline

A conveyor belt in a car manufacturing plant is an example of a pipeline
if there are many cars being built at the same time.
There can be many workers working on different cars at the same time,
but each worker always performs the same step.
One worker might, for example, only always attach the left front tire.
Once this step is done, the car moves forward on the conveyor belt.

In a pipeline, each rank performs a single step in a process with many steps.
Data flows trough the pipeline and gets modified along the way.
Naturally a pipeline is only efficient if there is a large amount of data
to feed into it.
The different stages cannot work on the same piece of data at the same time.

The main downside of a pipeline is that some of the ranks may spend time
waiting for data from the previous step.
It's important to balance the steps to minimize the wait time.


### Domain Decomposition

When the data is structured in a regular way, such as when
simulating atoms in a crystal, it makes sense to divide the space
into domains. Each rank will handle the simulation within it's
own domain.

![Data points divided to four ranks]({{ page.root }}{% link files/domaindecomposition.png %})


Many algorithms involve multiplying very large matrices.
These include finite element methods for computational field theories as well as
training and applying neural networks.
The most common parallel algorithm for matrix multiplicitation divides
the input matrices into smaller submatrices and composes the result from
multiplications of the submatrices.
If there are four ranks, the matrix is divided into four submatrices.

![A = A_{11} & A_{12} \\ A_{21} & A_{22} ]({{ page.root }}{% link fig/parallel_mmul.png %}){:height="100px"}
![B = B_{11} & B_{12} \\ B_{21} & B_{22} ]({{ page.root }}{% link fig/parallel_mmul_2.png %}){:height="100px"}
![A . B = A_{11} . B_{11} + A_{12} . B_{21} & A_{11} . B_{12} + A_{12} . B_{22} \\  A_{21} . B_{11} + A_{22} . B_{21} & A_{21} . B_{12} + A_{22} . B_{22} ]({{ page.root }}{% link fig/parallel_mmul_3.png %}){:height="100px"}

The ranks don't actually need all the data, but they do need a large amount.
Assuming that each rank holds one submatrix of A and B at the beginning, they need
to send all their data to two other processes.
If there were more that 4 ranks, they would need to share an entire row and a column.



## Communication Patterns

### Gather / Scatter

In gather type communication, all ranks send a piece of information to one rank.
Gathers are typically used when printing out information or writing to disk.
For example, each could send the result of a measurement to rank 0 and rank 0
could print all of them. This ensures that the information is printed in order.

Similarly, in a scatter communication, one rank one rank sends a piece of data to all the other ranks.
Scatters are useful for communicating parameters to all the ranks doing the computation.
The parameter could be read from disk but it could also be produced by a previous computation.

Gather and Scatter operations require more communication as the number of ranks increases and don't
tend to get slower when the number of ranks is large.
They have efficient implementations in the MPI libraries.

### Halo Exchange

![Halo Exchange]({{ page.root }}{% link files/haloexchange.png %}){:height="200px"}

A common feature of domain decomposed algorithms is that communications is limited to a small number
of other ranks that work on a domain a short distance away.
For example, in a simulation of atomic crystals, updating a single atom usually requires information 
of a couple of it's nearest neighbours.

In such a case each rank only needs a thin slice of data from it's neighbouring rank
and send the same slice from it's own data to the neighbour.
The data received from neighbours forms a "halo" around the the ranks data.


### Reduction

![Reduction]({{ page.root }}{% link files/reduction.png %}){:height="150px"}

A reduction is an operation that reduces a large amount of data, a vector or a matrix,
to a single number.
The sum examble above is a reduction.
Since data is needed from all ranks, this tends to be a time consuming operation, similar to
a gather operation.
Usually each rank first performs the reduction locally, arriving at a single number.
They then perform steps of collecting data from some of the ranks and performing the reduction
on that data, until all the data has been collected.
The most efficient implementation depends several technical features of the system.
Fortunately many common reductions are implemented in the MPI library and are often
optimised for a specific system.



### All to All

In other cases, some information needs to be sent from every rank to every other rank
in the system.
This is the most problematic scenario, the large amount of communication required reduces the
potential gain from designing a parallel algorithm.
Nevertheless the perfomance gain may be worth the effort if it is necessary to solve the
problem quickly.



> ## Pattern Examples
>
> The examples in the first four lessons are also examples of different communication
> patterns. Can you identify one or more pattern in each example?
>
{: .challenge}




{% include links.md %}

