---
title: "Parallel Paradigms"
teaching: 20
exercises: 10
questions:
- "How do I split the work?"
objectives:
- "Introduce standard communication and data storage patterns"
keypoints:
- "Several standard patterns: Trivial, Queue, Master / Worker, Domain Decomposition, All-to-All"
---


## Embarassingly Parallel Problems

Coming back to the examples of building a car out of a set of parts on a manufacturing line.
We can increase the number of cars produced in an amount of time by building more assemply
line and hiring more workers. The problem can be solved faster by running more independent
copies of the problem at once.

This known as an embarassingly parallel problem. No communication is needed between the processes.
This is the best case scenario, you don't need to desing a parallel algorithm to solve the
problem.

## Task Parallellism

Task parallellism is of the two main paradigms of parallel algorithms.
It refers to separating the process into small individual tasks and distributing
them accross the ranks.

If our problem is constructing a single car as quickly as possible, and not building many
cars quickly, we need to split the work in some way.
If we hire a large number of workers and buy tools for each of them,
we can identify different tasks that can be performed in parallel,
and give them out to the workers when they are free.

In MPI, we write the program by giving instructions for a single worker.
We need to make sure that the worker can find a task to execute and that
no other worker will try to do the same task.

### Queue

A task queue is a simple implementation of task parallellism.
Each worker will get tasks from a predefined queue, some of which
may depend on others being completed.
The tasks can be very different and take different amounts of time,
but when a worker has completed it's tasks, it will pick the next one
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

In an MPI implementation, main function will usually contain an if
statement that determines if the rank is the master or a worker.
The master will usually execute a completely different code from the workers.

Naturally the Master / Worker approach reserves a rank to be the master.
In a large application this is usually a small cost.
The larger cost is a bit more subtle. Because every node needs to communicate with
the master, the bandwidth of the master rank can become bottleneck.


## Data Parallellism

Data parallellism is the other main parallellisation paradigm. It refers to distributing
the data between the ranks. Each rank processes it's own portion of the input data and
communicates only the portion that is necessary to the other ranks.
Often all the ranks execute the same steps, just with a different set of data.

![Each rank has it's own data]({{ page.root }}{% link files/dataparallel.png %})

In car manufacturing, you migth split the parts between the workers and have them construct
subcomponents. However this would probably be inefficient in the final steps.
The second example, a sum over a set of numbers, is a better fit for a data parallel approach.
First we need to change the algorithm, to make it possible to execute in parallel.

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

### Domain Decomposition

When the data is structured in a regular way, such as when
simulating atoms in a crystal, it makes sense to divide the space
into domains. Each rank will handle the simulation within it's
own domain.

![Data points divided to four ranks]({{ page.root }}{% link files/domaindecomposition.png %})

Domain decomposition is a subset of data parallel algorithms.
In fact, the example of the sum algorithm uses domain decomposition.
It splits the long list of numbers into domains that each rank will work on.

A common feature of domain decomposed algorithms is that communications is limited to a small number
of other ranks that work on a domain a short distance away.
For example, in a simulation of atomic crystals, updating a single atom usually requires information 
of a couple of it's nearest neighbours.


### All to All

In other cases, some information needs to be sent from every rank to every other rank
in the system.
This is the most problematic scenario, the large amount of communication required reduces the
potential gain from designing a parallel algorithm.
Nevertheless the perfomance gain may be worth the effort if it is necessary to solve the
problem quickly.

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

> ## Pattern Examples
>
> The examples in the first four lessons are also examples of different communication
> patterns. Can you identify one or more pattern in each example?
>
{: .challenge}


## Performance
### Number of Transfers

The number of individual transfers is also a significant factor.
Each transfer will take some amount of time independent of the amount of data transferred.
The minimum time taken par transfer is known as the latency.
It depends significantly on the system.
If the ranks run on the same chip, the transfer is almost instantaneous.
If they run on different machines and communicate over the internet, this can be seconds.

### Surface to Volume Ratio

The ratio of serial regions to parallel regions is the most important factor in how well an algorithm can be parallellised.
What other factors are there?

The amount of data that needs to be transferred between the ranks is another important factor.
This is known as the surface to volume ratio.
The tradeoff is apparent in a domain decomposed system with nearest neighbour communication,
where a single rank is responsible for a given volume and
the data on the surface of that volume needs to be communicated.
The smaller the volume, the larger the surface is compared to the volume.

The time it takes to transfer a MB of data depends significantly on the network and
the distance between the ranks.



{% include links.md %}

