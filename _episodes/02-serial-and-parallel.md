---
title: "Serial and Parallel Regions"
teaching: 20
exercises: 10
questions:
- "When is parallel computing useful?"
- "How fast can my code run?"
objectives:
- "Understanding theoretical limitations on parallel algorithms"
keypoints:
- "Algorithms can have parallellisable and non-parallellisable sections"
- "A highly parallel algorithm may be slower on a single processor"
- "The theoretical maximum speed is determined by the serial sections"
- "The other main restruction is communication speed between the processes"
---



>## Serial and Parallel regions
>
> Identify serial and parallel in a given code
>
{: .challenge}

>## Parallel loop
>
> Modify the following code to split the loops among processes
>
> > ## C
> > ~~~
> > #include <stdio.h>
> > #include <math.h>
> > #include <mpi.h>
> > 
> > main(int argc, char** argv) {
> >   int rank, n_ranks, numbers_per_rank;
> >   int my_first, my_last;
> >   int numbers = 10;
> >
> >   // Firt call MPI_Init
> >   MPI_Init(&argc, &argv);
> >   // Get my rank and the number of ranks
> >   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
> >   MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
> >
> >   // Calculate the number of iterations for each rank
> >   numbers_per_rank = ceiling(numbers/n_ranks);
> >   my_first = rank * numbers_per_rank;
> >   my_last = my_first + numbers_per_rank;
> >
> >   // Run only the part of the loop this rank needs to run
> >   // The if statement makes sure we don't go over
> >   for( int i=my_first; i<my_last; i++ ) if( i < numbers ) {
> >     print("I'm printing the number %d.", i);
> >   }
> >
> >   // Call finalize at the end
> >   MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> >
> {: .prereq .foldable}
> 
>
> > ## Fortran
> >
> >
> {: .prereq .foldable}
>
{: .challenge }

{% include links.md %}

