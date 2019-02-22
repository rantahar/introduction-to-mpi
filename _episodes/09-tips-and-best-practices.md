---
title: "Tips and Best Practices"
teaching: 20
exercises: 0
questions:
- "What best practices should I know before I write my program?"
objectives:
- "Introduce some generally useful patterns"
keypoints:
- "Chooce a parallel algorithm"
- "Think about how you split the work before you start"
- "Reduce the number of transfers by packing the data into buffers"
- "Use MPI collective functions"
- "Optimise communication first"
- "Try to interleave communication and computation"
- "Prefer blocking functions"
---

During the second half of the day we will use the methods from the morning sessions
to parallellise a more complex code.
We will focus on a workflow that will ensure the parallel implementation produces
correct results and work on optimising for speed and efficiency. 
But before digging into our serial code, let's wrap up the morning sessions by picking
some useful details and thinking about best practices.

The main takeaway should be that the library functions are not very complicated and 
the number of new operations you are likely to use is relatively small.
The complication arises from thinking about the ranks working together.
The most important decisions are the once you are likely to make early.

### Choose a Parallel Algorithm
Some algorithms are fast in serial, but cannot be made parallel, or are
inefficient in parallel.
If the algorithm has large serial regions, there is little hope.
A different algorithm may be less efficient in serial, but have more
parallel regions and less need for communication.

### Think About the Paradigm
How will I split the work? Will different ranks work on different tasks?
Will I split the data between the tasks, and if I do, how should I split it?
The way the task is distributed will determine how efficient the resulting
program can be and how well he runtime scales with the number of ranks.

### Start With a Serial Code
It's easy to make mistakes.
Start with a working serial code and continually compare between the
parallel and the serial versions.
Make changes a section at a time until the program is sufficiently
efficient.
Don't parallellise sections that have on effect on performance.

### Don't Optimise Prematurely

You should start with straightforward, easy to read and correct code.
Optimisation usually makes the code less readable can lead to mistakes.

## Don't Optimise Unnecessarily
Once you have a working code only optimise the important parts.
Use a profiler to determine where the program spends its time.
Spend you time on the same lines of code.
Keep everything else nice, clean and readable.


### Use Packed Transfers
At a slightly lower level, it is also important to optimise the data transfers.
Each transfer has a latency, it takes time to set up, to move the data
between the ranks and to write it to the buffer.
It's more efficient to pack as much data as possible into each transfer
and reduce the number of individual transfers.

### Use MPI Collective Operations
Using the MPI functions for collective communication is almost always more
efficient.
They can be implemented specifically for the machine you're running on,
and may use hardware you don't have direct control over.

### Optimise Communication Over Computation
Especially when running on a large system, it is more important to reduce 
communication than it is to avoid computation. The processor can run quite a 
few floating point operations in the time it takes for the network to send
a number from one rank to the other.

### Interleave Communication and Computation
When communication time starts to become a significant bottleneck, think about
what is the earliest time you can send the data and the latest time you need it
start transfers early and do as much work as possible while the data is moving
from one rank to another.

### Prefer Blocking Receives
Use blocking receives when it doesn't affect performance.
They make it much easier to keep track of things.

## Choose the Right MPI 
There are multiple implementations of the MPI standard.
They are often optimised for a specific set of machines.
In an HPC system, it's worth using one that's designed for that system.

### Post Receives Early
When performance really matters, post receives early.
You can only post the send once you have the data.
You can post a receive when ever you want.
There is no reason for a communication to be waiting for a
receive to be posted.

Do this within reason. Don't start all your transfers at the beginning of the program.

{% include links.md %}

