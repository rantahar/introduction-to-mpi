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
> Let's get our hands dirty from the start and make sure mpi is installed. Open a bash command line and run the simple command.
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
> What did mpirun do?
>
{: .challenge}

Just running a program with mpirun starts several copies of it.
The number of copies is decided by the -n parameter.
In the example above, the program does not know it was started by mpirun
and each copy just works as if they were the only one.

For the copies to work together, they need to know about their role in the compution.
This usually also requires knowing the total number of tasks running at the same time.

To achieve this, the program needs to call the 
MPI_Init function in C, or the MPI_INIT function in Fortran.
This will setup the environment for MPI and assing a number, called rank, to each process.
At the end, each process should also cleanup by calling MPI_Finalize or MPI_FINALIZE.

Between these two statements, you can find out the rank of the copy using the MPI_Comm_Rank function.

In C this is called as
~~~
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
~~~
{: .source}

In Fortran it is
~~~
integer rank
call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
~~~
{: .source}

Here's a small example:
> ## C
> ~~~
> #include <stdio.h>
> #include <mpi.h>
> 
> main(int argc, char** argv) {
>   int rank, n_ranks, numbers_per_rank;
>
>   // Firt call MPI_Init
>   MPI_Init(&argc, &argv);
>   // Get my rank and the number of ranks
>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>
>   printf("My rank number is = %d\n", rank);
>
>   // Call finalize at the end
>   MPI_Finalize();
> }
> 
> ~~~
> {: .output}
{: .solution}

> ## Fortran
> ~~~
>
>      program hello
>
>      include 'mpif.h'
>      
>      integer rank
>
>      call MPI_INIT(ierr)
>      call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
>      write(6,*) 'My rank number is ", rank
>      call MPI_FINALIZE(ierr)
>
>      stop
>      end
>
> ~~~
> {: .output}
{: .solution}

> ## Compile
>
> Compile and run the above code
>
{: .challenge}

Notice the MPI_COMM_WORLD parameter. This is an MPI communicator and was created by MPI_Init. It labels the set of cores, called ranks, we are working with. 

Notice also that we are still only coding for a single copy of the program. This is always true when using MPI. Each copy runs the same code and only differs by it's rank number.
The best way to think about writing MPI code is to focus on what a single rank
needs to be doing.
When all ranks are doing their job, the algorithm will work correctly.

Usually the rank will need to know how many other ranks there are. You can find this out using the MPI_Comm_size in C or MPI_COMM_SIZE in Fortran.

> ## Hello World!
>
> Use the mpi library to write a "Hello World" program.
> Each copy of the program, or rank, should print "Hello World!" followed by it's rank number.
> They should also print the total number of ranks.
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
> >      integer rank, n_ranks
> >
> >      call MPI_INIT(ierr)
> >      call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
> >      call MPI_COMM_SIZE(MPI_COMM_WORLD,n_ranks,ierr)
> >      write(6,*) 'Hello! World = ', rank
> >      write(6,*) 'total no. of nodes = ", n_ranks
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

In lesson 3 you will learn how to communicate between the ranks. From there, you can in principle write general parallel programs. The trick is in designing a working, fast and efficient parallel algorithm for you problem.


{% include links.md %}

