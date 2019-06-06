---
title: "Tips and Best Practices"
teaching: 20
exercises: 0
questions:
- "What best practices should I know before I write my program?"
objectives:
- "Useful tips and best practices"
keypoints:
---

## Design

* **Choose a Parallel Algorithm**

Some algorithms are fast in serial, but cannot be made parallel, or are
inefficient in parallel.
If the algorithm has large serial regions, there is little hope.
A different algorithm may be less efficient in serial, but have more
parallel regions and less need for communication.


* **Think About the Paradigm**

How will I split the work? Will different ranks work on different tasks?
Will I split the data between the tasks, and if I do, how should I split it?
The way the task is distributed will determine how efficient the resulting
program can be and how well the run time scales with the number of ranks.


* **Start With a Working Serial Code**

It's easy to make mistakes.
Start with a working serial code and continually compare between the
parallel and the serial versions.
Make changes a section at a time until the program is sufficiently
efficient.
Don't spend time parallellising sections that have no effect on performance.

* **Test**

Write tests and make sure they don't fail when making changes.

## Optimisation

* **Don't Optimise Prematurely**

You should start with straightforward, easy-to-read, correct code.
Optimisation usually makes the code less readable can lead to mistakes.


* **Don't Optimise Unnecessarily**

Once you have a working code only optimise the important parts.
Use a profiler to determine where the program spends its time.
Spend your time on the same lines of code.
Keep everything else nice, clean and readable.


* **Optimise Communication Over Computation**

Especially when running on a large system, it is more important to reduce 
communication than it is to avoid computation. The processor can run quite a 
few floating point operations in the time it takes for the network to send
a number from one rank to the other.


* **Interleave Communication and Computation**

When communication time starts to become a significant bottleneck, think about
what is the earliest time you can send the data and the latest time you need it.
Start transfers early and do as much work as possible while the data is moving
from one rank to another.


* **Use Packed Transfers**

At a slightly lower level, it is also important to optimise the data transfers.
Each transfer has a latency, it takes time to set up, to move the data
between the ranks and to write it to the buffer.
It's more efficient to pack as much data as possible into each transfer
and reduce the number of individual transfers.

* **Use MPI Collective Operations**

Using the MPI functions for collective communication is almost always more
efficient.
They can be implemented specifically for the machine you're running on,
and may use hardware you don't have direct control over.


* **Choose the Right MPI**

There are multiple implementations of the MPI standard.
They are often optimised for a specific set of machines.
In an HPC system, it's worth using one that's designed for that system.



* **Post Receives Early**

When performance really matters, post receives early.
You can only post the send once you have the data, but
you can post a receive when ever you want.
This allows the communication device to set up a buffer and prepare
to receive the message.
It also avoid the other rank waiting for the receive when it already
has the data.
receive to be posted.

Do this within reason. Don't start all your transfers at the beginning of the program.

{% include links.md %}

