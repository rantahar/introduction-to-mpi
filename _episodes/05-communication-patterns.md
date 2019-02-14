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

### Nearest neighbour communication


### All to All


## Trivially Parallel

A problem is trivially parallel if no communication is needed between the processes at any point.
While there are many such algorithms, parallel programmin is not needed to implement them.


> ## Pattern Examples
>
> The examples in the first four lessons are also examples of different communication
> patterns. Can you identify one or more patterns in each example?
>
{: .challenge}


> ## Patterns Relevant to Your Field
>
> If you brought your own research code, how is it implemented?
> What data is involved in each step of the algorithm.
> Think about what communication patterns 
>
> Think of computational research problems in your field.
> Go trough the same set of questions with a common
> computational problem.
>
{: .challenge}


{% include links.md %}

