---
title: "Parallellising a Serial Code"
teaching: 30
exercises: 30
questions:
- "What is the best way to write a parallel code?"
- "How do I parallellise my serial code?"
objectives:
- "Go through practical steps for going from a serial code to a parallel code"
- "Allow students to work on their own code or on example codes"
keypoints:
- "Start from a working serial code"
- "Write a parallel implementation for each function or parallel region"
- "Connect the parallel regions with a minimal amount of communication"
- "Continuously compare with the working serial code"
---

## Going from Serial to Parallel

The examples used in the previous sections were perhaps not the most realistic.
In this section we will look at a more complete code and take it from serial to
parallel a couple of steps.
First, download the serial code for solving the Poisson equation in
[C]({{ page.root }}{% link code/poisson/poisson.c %})
or in
[Fortran]({{ page.root }}{% link code/poisson/poisson.f %})
and familiarise yourself with what how it works.

If you have your own serial code, it's advisable to choos a small
section of the code and go trough the same steps.

### Parallel regions

The first step is to identify parts of the code that
can be written in parallel.
Go trough the algorithm and decide for each region if the data can be partitioned for parallel execution
or if certain tasks can be separated and run in parallel.

Can you find large or time consuming serial regions?
The sum of the serial regions gives the minimum amount of time it will take to run the program.
If the serial parts are a significant part of the algorithm, it may not be possible to write an efficient parallel a version.
Can you replace the serial parts with a different, more parallel algorithm?


> ## Parallel Regions
>
> Identify any parallel and serial regions in the code.
> What would be the optimal parallellisation strategy?
>
>> ## Solution
>>
>> There are no clearly identifiable parallel tasks.
>> The loops over space can be run in parallel.
>> There are parallisable loops in
>> * the setup, when initialising the fields
>> * the calculation of the time step, unew
>> * the difference, unorm
>> * overwriting the field u
>> * writing the files could be done in parallel
>>
>> The best strategy would seem to be parallellising the loops.
>> This will lead to a data parallel implementation with the
>> elements of the fields u and rho divided across the ranks.
>>
>> The 
>> 
>{: .output}
>
{: .challenge}


## Communication Patterns

If you decide it's worth your time to try to parallelise the problem,
the next step is to decide how the ranks will share the tasks or the data.
This should be done for each region separately, but taking into account the time it would take to reorganise the data if you decide to change the pattern between regions.

The parallellisation strategy is often based on the
underlying problem the algorithm is dealing with.
For example in materials science, it makes sense
to decompose the data into domains by splitting the
physical volume of the material.

Most programs will use the same pattern in every region.
There is a cost to reorganising data, mostly having to do with communicating large blocks between ranks.
This is really only a problem if done in a tight loop, many times per second.


> ## Communications Patterns
>
> How would you divide the data to the ranks?
> When does each rank need data from the other
> ranks?
> Which ones?
>
>> Only one of the loops requires data from the other ranks,
>> and these are only nearest neighbours.
>>
>> Parallellising the loops would actually be the same thing as splitting the physical volume.
>> Each iteration of the loop accesses one element
>> in the rho and uner fields and four elements in
>> the u field.
>> The u field needs to be communicated if the value
>> is stored on a different rank.
>>
>> There is also a global reduction for calculating unorm.
>> Every node needs to know the result.
>>
>{: .output}
>
{: .challenge}


## Set up tests

In a larger program individual tasks should already be
separated into functions.
Write a set of tests for the function you are planning to edit.
Use small units. Smaller units are easier to test.

> ## Write a Test
>
> Write a test for an individual iteration of the
> Poisson solver
>

## Write a Version for One Rank

Now write a version of the code that performs the
tasks of one node.
Communicate data in the simplest possible way,
probably just after it's created or just before it's needed.
Use blocking or non-blocking communication,
which ever you feel is simple. If you use non-blocking functions, call wait immediately.

The point is to write a simple code that works correctly.
You can optimise later.

> ## Parallel Execution
> 
> Let's parallellise split the outer loop across 
> the ranks.
> Write a program that permorms the iterations from
> j=rank*(MAX/n_ranks) to j=(rank+1)*(MAX/n_ranks).
> 
> Make sure the test works in parallel.
>
> It should not be necessary to do any communication.
>
{: .challenge}

> ## Communication
> 
> Add some communication and test two iterations.
>
{: .challenge}

## Optimisation

Now it's time to think about optimising the code.
Try it with a different numbers of ranks, up to
the maximum number you expect to use.
A parallel profiler is useful for finding the places 
where communication is taking time.

It's also good to check the speed with just one rank.
In most cases it can be made as fast as a serial code
using the same algorithm.

> ## Scaling
>
> Try running your program with an increasing number of ranks.
> Does the result change? What are the limitations on scaling?
>
{: .challenge}

> ## Parallel Execution
>
> We could parallellise the inner loop as well. Would this be efficient?
>
>
{: .challenge}

{% include links.md %}

