---
title: "Introduction to Parallel Computing"
teaching: 20
exercises: 10
questions:
- "What is parallel computing?"
- "What HPC system do I have access to?"
objectives:
- "Introducing parallel computing."
- "Use mpicc/mpifort and mpirun."
keypoints:
- "Many problems can be distributed across several processors and solved faster"
- "Mpirun runs copies of the program."
- "The copies are separated MPI rank"
---

In many fields of science we need more computational power than a single core on a single processor can provide.
In essence, parallel computing means simply dividing work between more than one computer or more than one core.
This way many problems can be solved faster, in time scales that make sense for research projects or study programs.
How efficiently a problem can be solved in parallel depends on the data and memory needs of the algorithm,
on how much the cores need to communicate with each other while working together.
The number of cores we can efficiently use also depends on the problem.
You probably have two or four on your laptop, and many problems can be run very efficiently on all of those cores.
As a researcher, you probably have access to an HPC system with thousands or hundreds of thousands of cores.
To use them all efficiently would be challenging in almost any field.

During this course you will learn use to design parallel algorithms and write parallel programs using the MPI library.
MPI stands for Message Passing Interface, and is a low level, extremely flexible and simple set of commands for communicating between copies of a program.

> ## Running with mpirun
>
> Let's get our hands dirty from the start and make sure mpi is installed. Open a bash command line and run a simple command.
> ~~~
> echo Hello World!
> ~~~
> {: .source}
>
> Now use mpirun to run the same command:
> ~~~
> mpirun -n 4 echo Hello World!
> ~~~
> {: .source}
>
> > 
> > ~~~
> > {: .output}
> {: .solution}
>
{: .challenge} 

The first two basic calls every MPI application will contain are MPI_Init and MPI_Finalize, (called MPI_INIT and MPI_FINALIZE in Fortran).

> ## Hello World!
>
> Let's get our hands dirty from the start and make sure everything is installed correctly.
> Use the mpi library to write a "Hello World" program. Each copy of the program should print its rank.
>
> > ## Solution in C
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > main(int argc, char** argv) {
> >   int rank, n_ranks, numbers_per_rank;
> >
> >   // Firt call MPI_Init
> >   MPI_Init(&argc, &argv);
> >   // Get my rank and the number of ranks
> >   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> >
> >   printf("Hello! World = %d\n", rank);
> >   printf("total no. of nodes = %d\n", n_ranks);
> >
> >   // Call finalize at the end
> >   MPI_Finalize();
> > }
> > 
> > ~~~
> > {: .output}
> {: .solution}
> 
> > ## Solution in Fortran
> > ~~~
> >
> >      program hello
> >
> >      include 'mpif.h'
> >
> >      call MPI_INIT(ierr)
> >      call MPI_COMM_RANK(MPI_COMM_WORLD,node,ierr)
> >      write(6,*) 'Hello! World = ', node
> >      call MPI_FINALIZE(ierr)
> >
> >      stop
> >      end
> >
> > ~~~
> > {: .output}
> {: .solution}
>
{: .challenge} 


{% include links.md %}

