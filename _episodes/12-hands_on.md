---
title: "(Optional) Do it yourself"
teaching: 0
exercises: 70
questions:
- "What is the best way to write parallel code from serial?"
objectives:
- "Work on a second example, possibly your own research code."
keypoints:
- "Start from a working serial code."
- "Write a parallel implementation for each function or parallel region."
- "Connect the parallel regions with a minimal amount of communication."
- "Continuously compare with the working serial code."
---

>## Going from Serial to Parallel
>
>Choose a working serial code to work on. It can be your own research code, or
>the example below. Choose a function or a compact region
>of the code and write a parallel version of it.
>
>Follow the same steps as in lesson 9:
> 1. Identify parallel and serial regions.
>    Decide if the potential speedup is worth the effort.
> 2. Decide how to split the data among a number of ranks.
>    Think about how much data needs to be communicated
>    and how often.
> 3. Set up tests. Write a program that runs the function
>    with different parameters and checks that it's
>    running correctly.
> 4. Write the parallel code. When using MPI, this means
>    writing the function that will run on a single rank
>    and communicate with the others.
>    If you can, split this into multiple parts
>    for different kinds on input.
> 5. Profile when the program runs correctly.
> 6. Optimise, making sure the tests still succeed. Goto: 5.
>
> <br/>
> **Serial code for the Ising Model**
>
> As an example, here is the
> [code](../code/ising/ising2d4.c){: .show-c}
> [code](../code/ising/ising2d4.F08){: .show-fortran}
> [code](../code/ising/ising2d4.py){: .show-python}
> for simulating the Ising model.
>
> The Ising model describes the behaviour of a ferromagnetic
> material. The material is formed of atoms arranged in a
> 2 dimensional square lattice. Each atom has a magnetic field
> pointing either up or down. The atoms only interact with their
> immediate neighbours. When neighbouring atoms point in
> different directions, they increase the energy of the system
> a bit. Similarly when they point to different directions, they
> decrease the energy by a bit.
>
> At zero temperature the system would just fall to the state with
> lowest energy, but with a nonzero temperature it will sample
> states with different energies. Increasing energy is still
> alway less likely than decreasing it. Precisely, the probability
> of changing the direction of an atom is:
>
> $$ P = \min\left( e^{-\frac 1T \Delta_E}, 1 \right), $$
>
> Where $$\Delta_E$$ is the change in energy.
> The code runs multiple iterations of a loop over all atoms in the
> lattice and attempts to flip the direction of each one at a time.
> After enough iterations, the system should find a natural equilibrium
> for the given temperature.
>
> The code reads parameters from a file called `parameter`. It needs
> to contain two lines for two parameters, `beta` and `iter`.
> For example:
> ~~~
> beta 0.5
> iter 100
> ~~~
> {: .source}
>
>
>>## solution
>>
>> A possible MPI implementation for reference:
>> [ising2d4_mpi.c](../code/ising/ising2d4_mpi.c){: .show-c}
>> [ising2d4_mpi.F08](../code/ising/ising2d4_mpi.F08){: .show-fortran}
>> [ising2d4_mpi.py](../code/ising/ising2d4_mpi.py){: .show-python}
>{: .solution}
>
{: .challenge}


{% include links.md %}
