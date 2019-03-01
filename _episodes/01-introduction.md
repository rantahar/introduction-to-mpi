---
title: "Introduction to Parallel Computing"
teaching: 20
exercises: 20
questions:
- "How do I run a program in parallel?"
objectives:
- "Introduce parallel computing."
- "Use mpicc/mpifort and mpirun."
keypoints:
- "Many problems can be distributed across several processors and solved faster"
- "Mpirun runs copies of the program."
- "The copies are separated MPI rank"
---

## Parallel Computing

In many fields of science we need more computational power than a single core on a single processor can provide.
In essence, parallel computing means simply using more than one computer or more than one core to solve a problem faster.
Naively, using more CPU's (or cores) means that one can solve a problem much faster, in time scales that make sense for research projects or study programs.
However, how efficiently a problem can be solved in parallel depends on how the problem is divided among each CPU's (or cores) and the data and memory needs of the algorithm. This determines how much the cores need to communicate with each other while working together.
The number of cores we can efficiently use also depends on the problem.
You probably have two or four on your laptop, and many problems can be run very efficiently on all of those cores.
As a researcher, you probably have access to an HPC system with thousands or hundreds of thousands of cores.
To use them all efficiently would be challenging in almost any field.

During this course you will learn to design parallel algorithms and write parallel programs using the MPI library.
MPI stands for Message Passing Interface, and is a low level, extremely flexible and simple set of commands for communicating between copies of a program.

## Using MPI

> ## Running with mpirun
>
> Let's get our hands dirty from the start and make sure mpi is installed. Open a bash command line and run the simple command.
> ~~~
> echo Hello World!
> ~~~
>{: .language-bash}
>
> Now use mpirun to run the same command:
> ~~~
> mpirun -n 4 echo Hello World!
> ~~~
> {: .language-bash}
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
{% highlight C %}
int rank;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
{% endhighlight %}

In Fortran it is
{% highlight Fortran %}
integer rank
call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
{% endhighlight %}

Here's a more complete example:
> ## C
>
>~~~
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
>~~~
>{: .source .language-c}
>
{: .prereq .foldable}

> ## Fortran
>
> Fortran examples, exercises and solutions in this workshop will conform to the 
> Fortran 2008 standard.
> The standard can be specified to the compiler in two ways:
> * using the .f08 file extention
> * adding -std=f2008 on the command line.
>
>~~~
>program hello
>
>    use mpi
>    implicit none
>     
>    integer rank, ierr
>
>    call MPI_INIT(ierr)
>    call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
>    write(6,*) "My rank number is ", rank
>    call MPI_FINALIZE(ierr)
>
>end
>~~~
>{: .source .language-fortran}
>
{: .prereq .foldable}

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

## Hello World!
> ## Hello World!
>
> Use the mpi library to write a "Hello World" program.
> Each copy of the program, or rank, should print "Hello World!" followed by it's rank number.
> They should also print the total number of ranks.
>
>> ## Solution in C
>>
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> main(int argc, char** argv) {
>>   int rank, n_ranks, numbers_per_rank;
>>
>>   // Firt call MPI_Init
>>   MPI_Init(&argc, &argv);
>>   // Get my rank and the number of ranks
>>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>
>>   printf("Hello World! I'm rank %d\n", rank);
>>   printf("total no. of ranks = %d\n", n_ranks);
>>
>>   // Call finalize at the end
>>   MPI_Finalize();
>> }
>> ~~~
>>{: .source .language-c}
>{: .solution}
> 
>> ## Solution in Fortran
>> ~~~
>>program hello
>>
>>     use mpi
>>     implicit none
>>     
>>     integer rank, n_ranks, ierr
>>
>>     call MPI_INIT(ierr)
>>     call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
>>     call MPI_COMM_SIZE(MPI_COMM_WORLD,n_ranks,ierr)
>>     write(6,*) "Hello World! I'm rank ", rank
>>     write(6,*) "total no. of ranks = ", n_ranks
>>     call MPI_FINALIZE(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .solution}
>
{: .challenge}



>## Parallel loop
>
> Modify the following code to split the loops among processes
>
>> ## C
>> ~~~
>> #include <stdio.h>
>> 
>> main(int argc, char** argv) {
>>   int numbers = 10;
>>
>>   for( int i=1; i<numbers; i++ ) {
>>     print("I'm printing the number %d.", i);
>>   }
>>
>> }
>> ~~~
>> {: .source .language-c}
>{: .prereq .foldable}
>
>> ## Fortran
>> ~~~
>>program printnumbers
>>    implicit none
>>    
>>    integer numbers, number
>>    numbers = 10
>>
>>    do number = 0, numbers - 1
>>         write(6,*) "I'm printing the number", number
>>    end do 
>>end
>> ~~~
>> {: .source .language-fortran}
>{: .prereq .foldable}
>
>
>
>> ## Solution in C
>> ~~~
>> #include <stdio.h>
>> #include <math.h>
>> #include <mpi.h>
>> 
>> main(int argc, char** argv) {
>>   int rank, n_ranks, numbers_per_rank;
>>   int my_first, my_last;
>>   int numbers = 10;
>>
>>   // Firt call MPI_Init
>>   MPI_Init(&argc, &argv);
>>   // Get my rank and the number of ranks
>>   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
>>   MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
>>
>>   // Calculate the number of iterations for each rank
>>   numbers_per_rank = ceiling(numbers/n_ranks);
>>   my_first = rank * numbers_per_rank;
>>   my_last = my_first + numbers_per_rank;
>>
>>   // Run only the part of the loop this rank needs to run
>>   // The if statement makes sure we don't go over
>>   for( int i=my_first; i<my_last; i++ ) if( i < numbers ) {
>>     print("I'm printing the number %d.", i);
>>   }
>>
>>   // Call finalize at the end
>>   MPI_Finalize();
>> }
>> ~~~
>> {: .source .language-c}
>>
>{: .solution}
>
>
>> ## Solution in Fortran
>> ~~~
>>program print_numbers
>>
>>    use mpi
>>    implicit none
>>
>>    integer numbers, number
>>    integer rank, n_ranks, ierr
>>    integer numbers_per_rank, my_first, my_last
>>    numbers = 10
>>
>>    ! Call MPI_INIT
>>    call MPI_INIT(ierr)
>>
>>    ! Get my rank and the total number of ranks
>>    call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
>>    call MPI_COMM_SIZE(MPI_COMM_WORLD,n_ranks,ierr)
>>
>>    ! Calculate the number of iterations for each rank
>>    numbers_per_rank = numbers/n_ranks
>>    if (MOD(numbers, numbers_per_rank) > 0) then
>>         ! add 1 in case the number of ranks doesn't divide numbers
>>         numbers_per_rank = numbers_per_rank + 1
>>    end if
>>
>>    ! Figure out the first and the last iteration for this rank
>>    my_first = rank * numbers_per_rank
>>    my_last = my_first + numbers_per_rank
>>
>>    ! Run only the necessary part of the loop, making sure we don't go over
>>    do number = my_first, my_last - 1
>>         if (number < numbers) then
>>              write(6,*) "I'm printing the number", number
>>         end if
>>    end do
>>
>>
>>    call MPI_FINALIZE(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>>
>{: .solution}
>
>
{: .challenge }

In lesson 3 you will learn how to communicate between the ranks. From there, you can in principle write general parallel programs. The trick is in designing a working, fast and efficient parallel algorithm for you problem.


{% include links.md %}

