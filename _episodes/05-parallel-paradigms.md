---
title: "Parallel Paradigms"
teaching: 20
exercises: 10
questions:
- "How do I split the work?"
objectives:
- "Introduce Message Passing and Shared Memory"
- "Introduce Data Parallel and Task Parallel"
- "Introduce standard communication and data storage patterns"
keypoints:
- "Two major paradigms, message passing and shared memory."
- "MPI implements the Message Passing paradigm"
- "Several standard patterns: Trivial, Queue, Master / Worker, Domain Decomposition, All-to-All"
---

## Message Passing and Shared Memory

How to realize a parallel computing is roughly divided into two camps: one is "data parallel" and the other is "message passing". MPI (Message Passing Interface, the parallelization method we use in our lessons) obviously belongs to the second camp. "openMP" belongs to the first. In message passing paradigm, each CPU (or a core) runs an independent program. Parallelism is achieved by receiving data which it doesn't have and sending data which it has. In data parallel paradigm, there are many different data and operations (instructions in assembly language speaking) are performed on these data at the same time. Parallelism is achieved by how many different data a single operation can act on. This division is mainly due to historical development of parallel architectures: one follows from shared memory architecture like SMP (Shared Memory Processor) and the other from distributed computer architecture. A familiar example of the shared memory architecture is GPU (or multi-core CPU) architecture and a familiar example of the distributed computing architecture is cluster computer. Which architecture is more useful depends on what kind of problems you have. Sometimes, one has to use both!

### Embarassingly Parallel Problems

Coming back to the examples of building a car out of a set of parts on a manufacturing line.
We can increase the number of cars produced in a given amount of time by building more assembly
lines and hiring more workers. The problem can be solved faster by running more independent
copies of the problem at once.

This known as an embarassingly parallel problem. No communication is needed between the processes.
This is the best case scenario, you don't need to design a parallel algorithm to solve the
problem.

### Shared Memory

If our problem is constructing a single car as quickly as possible, and not building many
cars quickly, we need to split the work in some way.
If we hire a large number of workers and buy tools for each of them,
we can identify different tasks that can be performed in parallel,
and give them out to the workers when they are free.

#### Queue

A task queue is a simple implementation of task parallellism.
Each worker will get tasks from a predefined queue.
The tasks can be very different and take different amounts of time,
but when a worker has completed it's tasks, it will pick the next one
from the queue.

![Each rank taking one task from the top of a queue]({{ page.root }}{% link files/queue.png %})

In an MPI code,
the queue approach requires the ranks to communicate what they are doing to
all the other ranks, resulting in some communication overhead.
If several ranks are using the same data, communicating it can also create
some overhead.

#### Master / Worker

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

#### Pipeline

A conveyor belt in a car manufacturing plant is an example of a pipeline
if there are many cars being built at the same time.
There can be many workers working on different cars at the same time,
but each worker always performs the same step.
One worker might, for example, only always attach the left front tire.
Once this step is done, the car moves forward on the conveyor belt.

In a pipeline, each rank performs a single step in a process with many steps.
Data flows trough the pipeline and get's modified along the way.
Naturally a pipeline is only efficient if there is a large amount of data
to feed into it.
The different stages cannot work on the same piece of data at the same time.

The main downside of a pipeline is that some of the ranks may spend time
waiting for data from the previous step.
It's important to balance the steps to minimize the wait time.


### Message Passing

In the message passing paradigm, each processor runs it's own program and works on it's own data.
To work on the same problem in parallel, they communicate by sending messages to each other.

Imagine we hire workers to build a car as before, but instead of having them work in the same room
on the same car, we put them in separate room and have them work on different parts.
Worker 1 will build the back part of the car, worker 2 will build the middle part and
worker 3 will build the front.
At the end, workers 2 and 3 will load their finished producs onto a conveyer belt that delivers them
to worker 1, who will weld them together.

This may be an inefficient way to build a car, but is often a closer analogy to the way different 
processors work in a computer or a cluster.
Each processor, or a core, will have it's own cache, a small amount of memory it does not share
with the others.
Cores on the same chip will share some memory on the chip, analogous to the workers sharing a storeroom
they can take parts from.
Clusters have many chips connected with a fast network connection.
Compared to the speed at which a core can do computation, this is analogous to going to a different
building at the other side of the town.

In Shared Memory, if one rank modifies a piece of data, all the other ranks will see the change.
In Message passing this is not the case. Each rank has it's own data and modifying it will only
modify the data for that rank.

![Each rank has it's own data]({{ page.root }}{% link files/dataparallel.png %})

Let's say we need to calculate the sum of a large set of numbers.
Here is a simple parallel version of the algorithm.

~~~
sum = 0
split numbers into sublists

for sublist in sublists
   subsum = 0
   for number in sublist
      subsum = subsum + number
   sum = sum + subsum
~~~
{: .output}

We split the list of numbers into sublists and calculate the sum in the sublists separately.
At the and we calculate the sum of these sublists.
The sums over the sublists are independent of each other and can be done in parallel.

This algorithm is data parallel. We can have each rank perform the sum over its own sublist.
Each rank only needs it's own sublist and only needs to communicate it's subsum to the others
at the end.

#### Domain Decomposition

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

A common feature of domain decomposed algorithms is that communications is limited to a small number
of other ranks that work on a domain a short distance away.
For example, in a simulation of atomic crystals, updating a single atom usually requires information 
of a couple of it's nearest neighbours.

### Reduction

A reduction happens when one rank processes a large amount of data into only a few numbers
and only communicates these to the other ranks.
The algorithm for calculating a sum of numbers above performs a reduction.

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

