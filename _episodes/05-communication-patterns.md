---
title: "Communication Patterns"
teaching: 20
exercises: 10
questions:
- "How do you design a parallel algorithm"
- "How do I split the work"
- "How should I divide the data"
objectives:
- "Introducing standard communication patterns"
- "Think about your software and decide which patterns it uses / could use"
keypoints:
- "Several standard patterns: Trivial, Queue, Master / Worker, Nearest Neighbour, All-to-All"
---

<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

## Task Parallellism

Task parallellism is of the two main paradigms for splitting an algorithm.
It refers to separating the process into small individual tasks and distributing
them accross the ranks.

### Queue

A task queue is a simple implementation of task parallellism.
Each rank will run the tasks from a predefined queue, some of which
may depend on others being completed.
The tasks can be very different and take different amounts of time,
but when a rank has completed it's tasks, it will pick the next one
from the queue.

The queue approach requires the ranks to communicate what they are doing to
all the other ranks, resulting in some communication overhead.
If several ranks are using the same data, communicating it can also create
some overhead.

When the tasks are different and data needs to move around, or when little data
is neede for each task, the queue approach is efficient. Imbalances between the
tasks do not slow down execution.

### Master / Worker

The master / worker approach is a more flexible version of the queue method.
On the the ranks is designated master and sends tasks to the other ranks,
called workers.
The master rank can perform any serial parts of the program.
It can also optimise the order of the tasks the workers are running, making sure
the workers don't have to wait idle and minimising the need for communication.

Naturally the Master / Worker approach reserves a rank to be the master.
In a large application this is usually a small cost.
The larger cost is a bit more subtle. Because every node needs to communicate with
the master, the bandwidth of the master rank can become bottleneck.


## Data Parallellism

Data parallellism is the other main parallellisation paradigm. It refers to distributing
the data between the ranks. Each rank processes it's own portion of the input data and
communicates only the portion that is necessary to the other ranks.
Often all the ranks execute the same steps, just with a different set of data.

### Nearest Neighbour

Nearest neighbour communication is a subset of data parallel algorithms.
It applies when the data is structured in a regular way in space.
Some good examples are simulation of atoms in a structured crystal.
Simulating one atom in the crystal only requires information about each of it's neighbours.
So if a rank simulates a small local volume
it only need to communicate information about the atoms at
the borders of the volume to it's neigbours.



### All to All

In other cases, some information needs to be sent from every rank to every other rank
in the system.
This may still not be all the information the node has,
the algorithms can be designed to minimize the amount of data needed.

Many algorithms involve multiplying very large matrices.
These include finite element methods for computational field theories as well as
training and applying neural networks.
The most common parallel algorithm for matrix multiplicitation divides
the input matrices into smaller submatrices and composes the result from
multiplications of the submatrices.

$$ A = \begin{bmatrix} A_{11} & A_{12} \\\\ A_{21} & A_{22} \end{bmatrix}$$

$$ B = \begin{bmatrix} B_{11} & B_{12} \\\\ B_{21} & B_{22} \end{bmatrix}$$

$$ A\cdot B = \begin{bmatrix} A_{11}\cdot B_{11} + A_{12}\cdot B_{21} & A_{11}\cdot B_{12} + A_{12}\cdot B_{22} \\\\  A_{21}\cdot B_{11} + A_{22}\cdot B_{21} & A_{21}\cdot B_{12} + A_{22}\cdot B_{22} \end{bmatrix}$$

Clearly a large parts of the submatrices, but not all of it.
Each of the four nodes only needs to communicate with two other nodes.


## Trivially Parallel

A problem is trivially parallel if no communication is needed between the processes.
While there are many such algorithms, parallel programming is not needed to implement them.


> ## Pattern Examples
>
> The examples in the first four lessons are also examples of different communication
> patterns. Can you identify one or more patterns in each example?
>
{: .challenge}


> ## Patterns Relevant to Your Field
>
> If you brought your own research code, how is it implemented?
> What data is involved in each step of the algorithm?
> What tasks could be performed at the same time
> Which would be more appropriate, a task parallel or a data parallel approach?
> 
>
> Think of computational research problems in your field.
> Go trough the same set of questions with a common
> computational problem.
>
{: .challenge}


{% include links.md %}

