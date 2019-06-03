---
title: "Parallelising a Serial Code"
teaching: 30
exercises: 90
questions:
- "What is the best way to write a parallel code?"
- "How do I parallelise my serial code?"
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

The exercises and solutions are based on the following code:

~~~
{% include code/poisson.c %}
~~~
{: .language-c .show-c }

~~~
{% include code/poisson.f08 %}
~~~
{: .language-fortran .show-fortran }

If you have your own serial code, it's advisable to choose a small
section of the code and go through the same steps.

### Parallel regions

The first step is to identify parts of the code that
can be written in parallel.
Go through the algorithm and decide for each region if the data can be partitioned for parallel execution,
or if certain tasks can be separated and run in parallel.

Can you find large or time consuming serial regions?
The sum of the serial regions gives the minimum amount of time it will take to run the program.
If the serial parts are a significant part of the algorithm, it may not be possible to write an efficient parallel version.
Can you replace the serial parts with a different, more parallel algorithm?


> ## Parallel Regions
>
> Identify any parallel and serial regions in the code.
> What would be the optimal parallelisation strategy?
>
>> ## Solution
>>
>> The loops over space can be run in parallel.
>> There are parallisable loops in
>> * the setup, when initialising the fields
>> * the calculation of the time step, `unew`
>> * the difference, `unorm`
>> * overwriting the field `u`
>> * writing the files could be done in parallel
>>
>> The best strategy would seem to be parallelising the loops.
>> This will lead to a domain decomposed implementation with the
>> elements of the fields `u` and `rho` divided across the ranks.
>>
>{: .solution}
>
{: .challenge}

### Communication Patterns

If you decide it's worth your time to try to parallelise the problem,
the next step is to decide how the ranks will share the tasks or the data.
This should be done for each region separately, but taking into account the time it would take to reorganise the data if you decide to change the pattern between regions.

The parallelisation strategy is often based on the
underlying problem the algorithm is dealing with.
For example in materials science, it makes sense
to decompose the data into domains by splitting the
physical volume of the material.

Most programs will use the same pattern in every region.
There is a cost to reorganising data, mostly having to do with communicating large blocks between ranks.
This is really only a problem if done in a tight loop, many times per second.

> ## Communications Patterns
>
> How would you divide the data between the ranks?
> When does each rank need data from other ranks?
> Which ranks need data from which other ranks?
>
>> ## Solution
>>
>> Only one of the loops requires data from the other ranks,
>> and these are only nearest neighbours.
>>
>> Parallelising the loops would actually be the same thing as splitting the physical volume.
>> Each iteration of the loop accesses one element
>> in the `rho` and `uner` fields and four elements in
>> the `u` field.
>> The `u` field needs to be communicated if the value
>> is stored on a different rank.
>>
>> There is also a global reduction for calculating `unorm`.
>> Every node needs to know the result.
>>
>{: .solution}
>
{: .challenge}


### Set Up Tests

Write a set of tests for the function you are planning to edit.
The tests should be sufficient to make sure each function performs correctly
in any situation when all tests pass.
You should test all combinations of different types of parameters and preferably use random input.
Here the habit of modular programming is very useful. When the functions are small and have a small amount of possible input types, they are easy to test.

> ## Automated Testing
>
> There are automated unit testing frameworks for almost any programming language.
> Automated testing creatly simplifies the workflow of running your tests and
> verifying that the entire program is correct.
> The program here is still relatively simple, we are testing a single function,
> so writing a main function with a test case is enough.
>
>Check out the
>[cmocka](https://cmocka.org/)
>or 
>[cunit](http://cunit.sourceforge.net/)
>unit testing frameworks.
> {: .show-c}
>
> Check out the
> [fortran-unit-test](https://github.com/dongli/fortran-unit-test)
> unit testing frameworks.
> {: .show-fortran}
>
{: .callout}

> ## Write a Test
>
> Change the main function of the poisson code to check that the poisson_step function
> produces the correct result.
> Test at least the output after 1 step and after 10 steps.
>
> What would you need to check to make sure the function is absolutely correct?
> 
>> ## Solution
>> ~~~
{% include code/poisson_test.c %}
>> ~~~
>>{: .source .language-c}
>{: .solution .show-c }
>
>> ## Solution
>> Here we create a module for the poisson solver, including only the
>> subroutine for performing a single step.
>>~~~
{%comment%}{% include code/poisson_test.f08 %}{%endcomment%}
>>~~~
>>{: .source .language-fortran}
>{: .solution .show-fortran }
>
{: .challenge}

### Write a Parallel Function Thinking About a Single Rank

In the message passing framework, all ranks execute the same code.
The most straightforward way of approaching writing a parallel implementation
is to think about a single rank at a time.
What does this rank need to to and what infromation does it need to do it?

Communicate data in the simplest possible way
just after it's created or just before it's needed.
Use blocking or non-blocking communication, which ever you feel is simpler.
If you use non-blocking functions, call wait immediately.

The point is to write a simple code that works correctly.
You can optimise later.

> ## Parallel Execution
> 
> Let's split the outer loop across the ranks.
> Write a program that performs the iterations from
> `j=rank*(MAX/n_ranks)` to `j=(rank+1)*(MAX/n_ranks)`.
>
> First, just implement a single step.
> For this, you should not need to communicate the field `u`.
> You will need a reduction.
> 
> Make sure the test also works correctly in parallel.
> Each rank needs to create the part of the fields `u`, `unew` and `rho`
> it needs for its own part of the loop.
>
> After this step the first test should succeed and the second test
> should fail.
>
>> ## Solution
>>
>>~~~
{% include code/poisson_mpi_step1.c %}
>>~~~
>>{: .source .language-c}
>{: .solution .show-c}
>
>
>> ## Solution
>>
>>~~~
>>module poisson_solver
>>
>>   use mpi
>>   implicit none
>>
>>contains
>>
>>   subroutine poisson_step(u, unew, rho, MAX, hsq, unorm)
>>      integer, intent(in) :: MAX
>>      real, intent(inout), dimension (0:(MAX+1),0:(MAX+1)) :: u, unew
>>      real, intent(in), dimension (0:(MAX+1),0:(MAX+1)) :: rho
>>      real, intent(in) :: hsq
>>      double precision local_unorm
>>      double precision, intent(out) :: unorm
>>      integer my_j_max, n_ranks
>>      integer ierr, i, j
>>      
>>      ! Find the number of x-slices calculated by each rank
>>      ! The simple calculation here assumes that MAX is divisible by n_ranks
>>      call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>      my_j_max = MAX/n_ranks
>>
>>      ! Calculate one timestep
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
>>         enddo
>>      enddo
>>
>>      ! Find the difference compared to the previous time step
>>      local_unorm = 0.0
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            local_unorm = local_unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u>>(i,j))
>>         enddo
>>      enddo
>>
>>      call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )
>>
>>      ! Overwrite u with the new field
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            u(i,j) = unew(i,j)
>>         enddo
>>      enddo
>>
>>   end subroutine poisson_step
>>
>>end module
>>~~~
>>{: .source .language-fortran}
>>
>> In the test function, we need create a fields, from 0 to `my_j_max`.
>>~~~
>>module poisson_test
>>  use fruit
>>  use poisson_solver
>>  use mpi
>>  implicit none
>>
>>contains
>>
>>   subroutine test_poisson_step
>>      integer, parameter :: MAX=20
>>
>>      real u(0:(MAX+1),0:(MAX+1)), unew(0:(MAX+1),0:(MAX+1))
>>      real rho(0:(MAX+1),0:(MAX+1))
>>      real h, hsq, difference
>>      double precision unorm, residual
>>      integer i, j, rank, n_ranks, ierr
>>      integer my_j_max
>>
>>      ! Get my rank and the number of ranks
>>      call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>      call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>   
>>      ! Find the number of x-slices calculated by each rank
>>      ! The simple calculation here assumes that MAX is divisible by n_ranks
>>      my_j_max = MAX/n_ranks;
>>
>>      ! Run setup
>>      hsq = h*h
>>
>>      ! Initialise the u and rho field to 0 
>>      do j = 0, my_j_max+1
>>         do i = 0, MAX+1
>>            u(i,j) = 0.0
>>            rho(i,j) = 0.0
>>         enddo
>>      enddo
>>
>>      ! Test a configuration with u=10 at x=1 and y=1
>>      ! The coordinate x=1, y=1 is always in rank 0
>>      if (rank == 0) then
>>         u(1,1) = 10
>>      end if
>>
>>      ! Run a single iteration of the poisson solver
>>      call poisson_step( u, unew, rho, MAX, hsq, unorm )
>>      call assert_true( unorm == 112.5, "Test One Step")
>>
>>      ! Run 49 iterations of the poisson solver (to get to 50)
>>      do i = 1, 50
>>         call poisson_step( u, unew, rho, MAX, hsq, unorm )
>>      end do
>>
>>      difference = unorm - 0.0018388170223
>>      call assert_true( difference*difference < 1e-16, "Test 50 Steps")
>>
>>   end subroutine test_poisson_step
>>
>>end module poisson_test
>>~~~
>>{: .source .language-fortran}
>{: .solution .show-fortran}
>
{: .challenge}

> ## Communication
> 
> Add in the nearest neighbour communication.
> The second test should then succeed.
>
>> ## Solution
>> Each rank needs to send the values at `u[1]` down to `rank-1` and
>> the values at `u[my_j_max]` to `rank+1`.
>> There needs to be an exception for the first and the last rank.
>>
>>~~~
{% include code/poisson_mpi_step2.c %}
>>~~~
>>{: .source .language-c}
>{: .solution .show-c}
>
>
>> ## Solution
>> Each rank needs to send the values at `u(1)` down to `rank-1` and
>> the values at `u(my_j_max)` to `rank+1`.
>> There needs to be an exeption for the first and the last rank.
>>
>>~~~
>>module poisson_solver
>>
>>   use mpi
>>   implicit none
>>
>>contains
>>
>>   subroutine poisson_step(u, unew, rho, MAX, hsq, unorm)
>>      integer, intent(in) :: MAX
>>      real, intent(inout), dimension (0:(MAX+1),0:(MAX+1)) :: u, unew
>>      real, intent(in), dimension (0:(MAX+1),0:(MAX+1)) :: rho
>>      real, intent(in) :: hsq
>>      double precision local_unorm
>>      double precision, intent(out) :: unorm
>>      integer status(MPI_STATUS_SIZE)
>>      integer my_j_max, n_ranks, rank
>>      integer ierr, i, j
>>      
>>      ! Find the number of x-slices calculated by each rank
>>      ! The simple calculation here assumes that MAX is divisible by n_ranks
>>      call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>      my_j_max = MAX/n_ranks
>>
>>      ! We need the rank number for the nearest neighbour communication
>>      call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>
>>      ! Calculate one timestep
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
>>         enddo
>>      enddo
>>
>>      ! Find the difference compared to the previous time step
>>      local_unorm = 0.0
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            local_unorm = local_unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
>>         enddo
>>      enddo
>>
>>      call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )
>>
>>      ! Overwrite u with the new field
>>      do j = 1, my_j_max
>>         do i = 1, MAX
>>            u(i,j) = unew(i,j)
>>         enddo
>>      enddo
>>
>>      ! The u field has been changed, communicate it to neighbours
>>      ! With blocking communication, half the ranks should send first
>>      ! and the other half should receive first
>>      if (mod(rank,2) == 1) then
>>         ! Ranks with odd number send first
>>
>>         ! Send data down from rank to rank-1
>>         call MPI_Send( unew(1,1), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, ierr)
>>         ! Receive dat from rank-1
>>         call MPI_Recv( u(1,0), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, status, ierr)
>>
>>         if (rank < (n_ranks-1)) then
>>            call MPI_Send( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, ierr)
>>            call MPI_Recv( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, status, ierr)
>>         endif
>>
>>      else
>>         ! Ranks with even number receive first
>>
>>         if (rank > 0) then
>>            call MPI_Recv( u(1,0), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, status, ierr)
>>            call MPI_Send( unew(1,1), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, ierr)
>>         endif
>>
>>         if (rank < (n_ranks-1)) then
>>            call MPI_Recv( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, status, ierr)
>>            call MPI_Send( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, ierr)
>>         endif
>>      endif
>>
>>   end subroutine poisson_step
>>
>>end module
>>~~~
>>{: .source .language-fortran}
>>
>{: .solution .show-fortran}
>
{: .challenge}

### Optimise

Now it's time to think about optimising the code.
Try it with a different numbers of ranks, up to
the maximum number you expect to use.
In the next lesson we will talk about a parallel profiler that can produce
useful information for this.

It's also good to check the speed with just one rank.
In most cases it can be made as fast as a serial code
using the same algorithm.

When making changes to improve performance, keep running the test suite.
A fast but incorrect program is useless.

> ## Strong Scaling
>
> Strong scaling means increasing the number of ranks without
> changing the problem.
> An algorithm with good strong scaling behaviour allows you to
> solve a problem more and more quickly by throwing more cores
> at it.
> There are no real problems where you can achieve perfect strong scaling
> but some do get close.
>
> Write a main function that calls the poisson step 100 times and set `MAX=512`.
>
> Try running your program with an increasing number of ranks.
> Measure the time taken using the Unix `time` utility.
> What are the limitations on scaling?
>
>> ## Solution 
>>
>> Exact numbers depend on the machine you're running on, 
>> but with a small number of ranks (up to 4) you should
>> see the time decrease with the number of ranks.
>> At 8 ranks the result is a bit worse and doubling again to 16 has little effect.
>>
>> The figure below shows an example of the scaling with
>> `MAX=512` and `MAX=2048`.
>> ![A figure showing the result described above for MAX=512 and MAX=2048]({{ page.root }}{% link fig/poisson_scaling_plot.png %})
>>
>> In the example, which runs on a machine with two 20-core Intel Xeon Scalable CPUs,
>> using 32 ranks actually takes more time.
>> The 32 ranks don't fit on one CPU and communicating between
>> the the two CPUs takes more time, even though they are within the same machine.
>>
>> The communication could be made more efficient.
>> We could use non-blocking communication and do some of the computation
>> while communication is happening.
>>
>{: .solution}
>
{: .challenge}

> ## Weak Scaling
>
> Weak scaling refers to increasing the size of the problem
> while increasing the number of ranks.
> This is much easier than strong scaling and there are several
> algorithms with good weak scaling.
> In fact our algorithm for integrating the Poisson equation
> might well have perfect weak scaling.
>
> Good weak scaling allows you to solve much larger problems
> using HPC systems.
>
> Run the Poisson solver with and increasing number of ranks,
> increasing `MAX` with the number of ranks.
> How does it behave?
>
>> ## Solution
>> 
>> Depending on the machine you're running on, the 
>> runtime should be relatively constant.
>> Runtime increase if you need to use nodes that are
>> further away from each other in the network.
>>
>> The figure below shows an example of the scaling with
>> `MAX_X=128*n_ranks`.
>> ![A figure showing a slowly increasing amount of time as a function of n_ranks]({{ page.root }}{% link fig/poisson_scaling_plot_weak.png %})
>>
>> In this case all the ranks are running on the same node with
>> 40 cores. The increase in runtime is probably due to the
>> memory bandwidth of the node being used by a larger number of
>> cores.
>>
>{: .solution}
{: .challenge}

{% include links.md %}

