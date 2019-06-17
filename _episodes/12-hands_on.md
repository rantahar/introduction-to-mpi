---
title: "Serial to Parallel"
teaching: 0
exercises: 70
questions:
- 
objectives:
- "Work on a second example, possibly your own research code."
keypoints:
- "Start from a working serial code"
- "Write a parallel implementation for each function or parallel region"
- "Connect the parallel regions with a minimal amount of communication"
- "Continuously compare with the working serial code"
---

>## Going from Serial to Parallel
>
>Choose a working serial code to work on. It can be your own research code
>of one of the two examples below. Choose a function or a compact region
>of the code write a parallel version of it.
>
>Follow the same steps as in lesson 9:
> 1. Identify parallel and serial regions.
>    Decide if the potential speedup is worth the effort.
> 2. Decide how to split the data among a number of ranks.
>    Think about how much data needs to be communicated
>    and how often.
> 3. Set up tests. Write a program that runs the function
>    with different parameters and checks that it's
>    running corectly.
> 4. Write the parallel code. When using MPI, this means
>    writing the function that will run on a single rank
>    and communicate with the others.
>    If you can, split this into multiple parts for
>    for different kinds on input.
> 5. Profile when the program runs correctly.
> 6. Optimise, making sure the tests still succeed. Go to 5.
>
> ### Serial code for the Ising Model
>
> [ising2d4.c](../code/ising/ising2d4.c){: .show-c} 
> [ising2d4.f](../code/ising/ising2d4.f){: .show-fortran}
>
>
>>## solution
>>
>> [ising2d4.c](../code/ising/ising2d4_mpi.c){: .show-c} 
>> [ising2d4.f](../code/ising/ising2d4_mpi.f){: .show-fortran}
>{: .solution}
>
{: .challenge}


{% include links.md %}

