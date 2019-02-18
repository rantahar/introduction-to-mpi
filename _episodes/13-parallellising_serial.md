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

The examples and exercises used 

> ## Parallel Regions
>
> Download the [C]({{ page.root }}{% link code/poisson/poisson.c %})
> or [Fortran]({{ page.root }}{% link code/poisson/poisson.f %}) code
> provided and read trough the code.
> Identify any potential parallel regions.
>
> You may also wish to work with your own serial code. If so, try
> this and the following exercises with a small part of your own code.
>
{: .challenge}


> ## Communications Patterns
>
> Looking back at the Poisson solver.
> What can you efficiently divide the problem into multiple
> ranks?
> How often do you need to communicate data?
>
{: .challenge}


> ## Parallel Execution
>
> Now add MPI_Init and MPI Finalize to the poisson code.
>
> Let's parallellise one loop first. Using MPI_Send and
> MPI_Recv, parallellise the outermost loop (with index j).
>
{: .challenge}



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
> Parallellise the inner loop. How does it compare?
>
{: .challenge}

{% include links.md %}

