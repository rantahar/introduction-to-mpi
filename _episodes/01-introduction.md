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
- "Many problems can be distributed accross several processors and solved faster"
- "Mpirun runs copies of the program."
- "The copies are separated MPI rank"
---

> ## Hello World!
>
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

