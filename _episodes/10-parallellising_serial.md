---
title: "Parallelising a Serial Code"
teaching: 30
exercises: 30
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
First, download the serial code for solving the Poisson equation in
[C]({{ page.root }}{% link code/poisson/poisson.c %})
or in
[Fortran]({{ page.root }}{% link code/poisson/poisson.f08 %})
and familiarise yourself with what how it works.

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
>> There are no clearly identifiable parallel tasks.
>> The loops over space can be run in parallel.
>> There are parallisable loops in
>> * the setup, when initialising the fields
>> * the calculation of the time step, `unew`
>> * the difference, `unorm`
>> * overwriting the field `u`
>> * writing the files could be done in parallel
>>
>> The best strategy would seem to be parallelising the loops.
>> This will lead to a data parallel implementation with the
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
> Which ranks does each rank need to exchange data with?
>
>> ## Solution
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

In a larger program individual tasks should already be
separated into functions.
Write a set of tests for the function you are planning to edit.
Use small units. Smaller units are easier to test.

> ## Write a Test
>
> Write a test for one iteration of the Poisson solver.
> 
>> ## Solution in C
>>  
>> The Poisson step function could look like this:
>> ~~~
>>#include <stdio.h>
>>#include <math.h>
>>
>>#define MAX 20
>>
>>double poisson_step( 
>>   float u[MAX+2][MAX+2],
>>   float unew[MAX+2][MAX+2],
>>   float rho[MAX+2][MAX+2],
>>   float hsq
>> ){
>> double unorm;
>>
>> // Calculate one timestep
>> for( int j=1; j <= MAX; j++){
>>   for( int i=1; i <= MAX; i++){
>>       float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>>	    unew[j][i] =0.25*( difference - hsq*rho[j][i] );
>>   }
>> }
>>
>> // Find the difference compared to the previous time step
>> unorm = 0.0;
>> for( int j = 1;j <= MAX; j++){
>>   for( int i = 1;i <= MAX; i++){
>>     float diff = unew[j][i]-u[j][i];
>>     unorm +=diff*diff;
>>   }
>> }
>>
>> // Overwrite u with the new field
>> for( int j = 1;j <= MAX;j++){
>>   for( int i = 1;i <= MAX;i++){
>>     u[j][i] = unew[j][i];
>>   }
>> }
>>
>> return unorm;
>>}
>> ~~~
>> {:.source .language-c}
>>
>> Assuming the above is saved to `poisson_step.c`, the
>> the following will run a simple test
>> ~~~
>>#include <stdarg.h>
>>#include <stddef.h>
>>#include <setjmp.h>
>>#include <cmocka.h>
>>
>>#include "poisson_step.c"
>>
>>static void test_poisson_step(void **state) {
>>   float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
>>   float h, hsq;
>>   double unorm, residual, difference;
>> 
>>   /* Set variables */
>>   h = 0.1;
>>   hsq = h*h;
>> 
>>   // Initialise the u and rho field to 0 
>>   for( int j=0; j <= MAX+1; j++ ){
>>      for( int i=0; i <= MAX+1; i++ ) {
>>         u[j][i] = 0.0;
>>         rho[j][i] = 0.0;
>>      }
>>   }
>> 
>>   // Test a configuration with u=10 at x=1 and y=1
>>   u[1][1] = 10;
>> 
>>   // Test a single step
>>   unorm = poisson_step( u, unew, rho, hsq );
>>   assert_true( unorm == 112.5 );
>> 
>>   //Test 50 steps
>>   for( int i=1; i<50; i++){
>>     unorm = poisson_step( u, unew, rho, hsq );
>>   }
>>   difference = unorm - 0.001950 ;
>>   assert_true( difference*difference < 1e-12 );
>>}
>>
>>/* In the main function create the list of the tests */
>>int main(void) {
>>  const struct CMUnitTest tests[] = {
>>     cmocka_unit_test(test_poisson_step),
>>  };
>>
>>  // Call a library function that will run the tests
>>  return cmocka_run_group_tests(tests, NULL, NULL);
>>}
>> ~~~
>> {:.source .language-c}
>{: .solution}
>
>> ## solution in Fortran
>> Here we create a module for the poisson solver, including only the
>> subroutine for performing a single step.
>>~~~
>>module poisson_solver
>>
>>  implicit none
>>
>>contains
>>
>>   subroutine poisson_step(u, unew, rho, MAX, hsq, unorm)
>>      integer, intent(in) :: MAX
>>      real, intent(inout), dimension (0:(MAX+1),0:(MAX+1)) :: u, unew
>>      real, intent(in), dimension (0:(MAX+1),0:(MAX+1)) :: rho
>>      real hsq
>>      double precision, intent(out) :: unorm
>>      integer i, j
>>
>>      ! Calculate one timestep
>>      do j = 1, MAX
>>         do i = 1, MAX
>>            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
>>         enddo
>>      enddo
>>
>>      ! Find the difference compared to the previous time step
>>      unorm = 0.0
>>      do j = 1, MAX
>>         do i = 1, MAX
>>            unorm = unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
>>         enddo
>>      enddo
>>
>>      ! Overwrite u with the new field
>>      do j = 1, MAX
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
>> The following defines a simple test.
>>~~~
>>module poisson_test
>>  use fruit
>>  use poisson_solver
>>  implicit none
>>
>>contains
>>
>>   subroutine test_poisson_step
>>      integer, parameter :: MAX=100
>>
>>      real u(0:(MAX+1),0:(MAX+1)), unew(0:(MAX+1),0:(MAX+1))
>>      real rho(0:(MAX+1),0:(MAX+1))
>>      real h, hsq, difference
>>      double precision unorm, residual
>>      integer i, j
>>   
>>      ! Run setup
>>      hsq = h*h
>>
>>      ! Initialise the u and rho field to 0 
>>      do j = 0, MAX+1
>>         do i = 0, MAX+1
>>            u(i,j) = 0.0
>>            rho(i,j) = 0.0
>>         enddo
>>      enddo
>>
>>      ! Test a configuration with u=10 at x=1 and y=1
>>      u(1,1) = 10
>>
>>      ! Run a single iteration of the poisson solver
>>      call poisson_step( u, unew, rho, MAX, hsq, unorm )
>>      call assert_true( unorm == 112.5, "Test One Step")
>>
>>      ! Run a single iteration of the poisson solver
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
>{: .solution}
>
{: .challenge}

### Write a Single Rank Version

In the message passing framework, all ranks execute the same code.
The most straightforward way of approaching writing a parallel implementation
is to think about a single rank at a time.
What does this rank need to to and what infromation does it need to do it?

Communicate data in the simplest possible way,
probably just after it's created or just before it's needed.
Use blocking or non-blocking communication,
which ever you feel is simple. If you use non-blocking functions, call wait immediately.

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
>> ## Solution in C
>>
>> In `poisson_step.c`:
>> ~~~
>>#include <stdio.h>
>>#include <math.h>
>>#include <mpi.h>
>>
>>#define MAX 20
>>
>>
>>double poisson_step( 
>>   float u[][MAX+2],
>>   float unew[][MAX+2],
>>   float rho[][MAX+2],
>>   float hsq
>> ){
>>  double unorm, global_unorm;
>>  int n_ranks, my_j_max;
>> 
>>  /* Find the number of x-slices calculated by each rank */
>>  /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>  my_j_max = MAX/n_ranks;
>> 
>>  // Calculate one timestep
>>  for( int j=1; j <= my_j_max; j++){
>>    for( int i=1; i <= MAX; i++){
>>        float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>> 	    unew[j][i] =0.25*( difference - hsq*rho[j][i] );
>>    }
>>  }
>> 
>>  // Find the difference compared to the previous time step
>>  unorm = 0.0;
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      float diff = unew[j][i]-u[j][i];
>>      unorm +=diff*diff;
>>    }
>>  }
>> 
>>  // Use Allreduce to calculate the sum over ranks
>>  MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
>> 
>>  // Overwrite u with the new field
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      u[j][i] = unew[j][i];
>>    }
>>  }
>> 
>>  return global_unorm;
>>}
>>~~~
>>{: .source .language-c}
>>
>> The test file: 
>>~~~
>>#include <stdarg.h>
>>#include <stddef.h>
>>#include <setjmp.h>
>>#include <cmocka.h>
>>
>>#include "poisson_step.c"
>>
>>static void test_poisson_step(void **state) {
>>  float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
>>  float h, hsq;
>>  double unorm, residual;
>>  double difference;
>>  int rank, n_ranks, my_j_max;
>>
>>  /* Find the number of x-slices calculated by each rank */
>>  /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>  my_j_max = MAX/n_ranks;
>>
>>  /* Set variables */
>>  h = 0.1;
>>  hsq = h*h;
>>
>>  // Initialise the u and rho field to 0 
>>  for( int j=0; j <= my_j_max+1; j++ ){
>>     for( int i=0; i <= MAX+1; i++ ) {
>>        u[j][i] = 0.0;
>>        rho[j][i] = 0.0;
>>     }
>>  }
>>
>>  // Test a configuration with u=10 at x=1 and y=1
>>  // The actual x coordinate is my_j_max*rank + x
>>  // meaning that x=1 is on rank 0
>>  if( rank == 0 ){
>>     u[1][1] = 10;
>>  }
>>
>>  // Test one step
>>  unorm = poisson_step( u, unew, rho, hsq, my_j_max );
>>  assert_true( unorm == 112.5 );
>>
>>  // Test 50 steps
>>  // Expect this to fail
>>  for( int iteration=0; iteration<50; iteration++ ){
>>     unorm = poisson_step( u, unew, rho, hsq, my_j_max );
>>  }
>>  difference = unorm - 0.001950 ;
>>  assert_true( difference*difference < 1e-12 );
>>}
>>
>>/* In the main function create the list of the tests */
>>int main(int argc, char** argv) {
>>  int cmocka_return_value;
>>
>>  // First call MPI_Init
>>  MPI_Init(&argc, &argv);
>>
>>  const struct CMUnitTest tests[] = {
>>     cmocka_unit_test(test_poisson_step),
>>  };
>>
>>  // Call a library function that will run the tests
>>  cmocka_return_value = cmocka_run_group_tests(tests, NULL, NULL);
>>
>>  // Call finalize at the end
>>  MPI_Finalize();
>>
>>  return cmocka_return_value;
>>}
>>~~~
>>{: .source .language-c}
>{: .solution}
>
>
>> ## Solution in Fortran
>> Run the `j`-loop over a smaller slice. Here we have called the
>> size of the `j`-slice `my_j_max`.
>>
>> The calculation of `unorm` is a sum over all ranks.
>> Use `MPI_Allreduce` to get it right.
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
>>      call MPI_COMM_SIZE(MPI_COMM_WORLD, n_ranks, ierr)
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
>>      call MPI_COMM_SIZE(MPI_COMM_WORLD, n_ranks, ierr)
>>      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
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
>>      ! Run a single iteration of the poisson solver
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
>{: .solution}
>
{: .challenge}

> ## Communication
> 
> Add in the nearest neighbour communication and test the result after
> 50 iterations.
>
>> ## Solution in C
>> Each rank needs to send the values at `u[1]` down to `rank-1`B and
>> the values at `u[my_j_max]` to `rank+1`.
>> There needs to be an exception for the first and the last rank.
>>
>> In poisson_step.c:
>>~~~
>>#include <stdio.h>
>>#include <math.h>
>>#include <mpi.h>
>>
>>#define MAX 20
>>
>>
>>double poisson_step( 
>>   float u[][MAX+2],
>>   float unew[][MAX+2],
>>   float rho[][MAX+2],
>>   float hsq
>> ){
>>  double unorm, global_unorm;
>>  float sendbuf[MAX],recvbuf[MAX];
>>  MPI_Status mpi_status;
>>  int rank, n_ranks, my_j_max;
>> 
>>  /* Find the number of x-slices calculated by each rank */
>>  /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>  MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>  my_j_max = MAX/n_ranks;
>>   
>>  // Calculate one timestep
>>  for( int j=1; j <= my_j_max; j++){
>>    for( int i=1; i <= MAX; i++){
>>        float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>> 	    unew[j][i] =0.25*( difference - hsq*rho[j][i] );
>>    }
>>  }
>> 
>>  // Find the difference compared to the previous time step
>>  unorm = 0.0;
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      float diff = unew[j][i]-u[j][i];
>>      unorm +=diff*diff;
>>    }
>>  }
>>
>>  // Use Allreduce to calculate the sum over ranks
>>  MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
>> 
>>  // Overwrite u with the new field
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      u[j][i] = unew[j][i];
>>    }
>>  }
>>
>>  // The u field has been changed, communicate it to neighbours
>>  // With blocking communication, half the ranks should send first
>>  // and the other half should receive first
>>  if ((rank%2) == 1) {
>>    // Ranks with odd number send first
>>
>>    // Send data down from rank to rank-1
>>    for( int i=0;i < MAX;i++){
>>      sendbuf[i] = unew[1][i+1];
>>    }
>>    MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD);
>>    // Receive dat from rank-1
>>    MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD,&mpi_status);
>>    for( int i=0;i < MAX;i++){
>>      u[0][i+1] = recvbuf[i];
>>    }
>>     
>>    if ( rank != (n_ranks-1)) {
>>      // Send data up to rank+1 (if I'm not the last rank)
>>      for( int i=0;i < MAX;i++){
>>        sendbuf[i] = unew[my_j_max][i+1];
>>      }
>>      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD);
>>      // Receive data from rank+1
>>      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD,&mpi_status);
>>      for( int i=0;i < MAX;i++){
>>        u[my_j_max+1][i+1] = recvbuf[i];
>>      }
>>    }
>>  
>>  } else {
>>    // Ranks with even number receive first
>>
>>    if (rank != 0) {
>>      // Receive data from rank-1 (if I'm not the first rank)
>>      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD,&mpi_status);
>>      for( int i=0;i < MAX;i++){
>>        u[0][i+1] = recvbuf[i];
>>      }
>>	     
>>      // Send data down to rank-1
>>      for( int i=0;i < MAX;i++){
>>        sendbuf[i] = unew[1][i+1];
>>      }
>>      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD);
>>    }
>>
>>    if (rank != (n_ranks-1)) {
>>      // Receive data from rank+1 (if I'm not the last rank)
>>      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD,&mpi_status);
>>      for( int i=0;i < MAX;i++){
>>        u[my_j_max+1][i+1] = recvbuf[i];
>>      }
>>      
>>      // Send data up to rank+1
>>      for( int i=0;i < MAX;i++){
>>        sendbuf[i] = unew[my_j_max][i+1];
>>      }
>>      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD);
>>    }
>>  }
>>
>>  return global_unorm;
>>}
>>~~~
>>{: .source .language-c}
>{: .solution}
>
>> ## Solution in Fortran
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
>>      call MPI_COMM_SIZE(MPI_COMM_WORLD, n_ranks, ierr)
>>      my_j_max = MAX/n_ranks
>>
>>      ! We need the rank number for the nearest neighbour communication
>>      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
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
>>         call MPI_SEND( unew(1,1), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, ierr)
>>         ! Receive dat from rank-1
>>         call MPI_RECV( u(1,0), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, status, ierr)
>>
>>         if (rank < (n_ranks-1)) then
>>            call MPI_SEND( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, ierr)
>>            call MPI_RECV( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, status, ierr)
>>         endif
>>
>>      else
>>         ! Ranks with even number receive first
>>
>>         if (rank > 0) then
>>            call MPI_RECV( u(1,0), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, status, ierr)
>>            call MPI_SEND( unew(1,1), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, ierr)
>>         endif
>>
>>         if (rank < (n_ranks-1)) then
>>            call MPI_RECV( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, status, ierr)
>>            call MPI_SEND( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, ierr)
>>         endif
>>      endif
>>
>>   end subroutine poisson_step
>>
>>end module
>>~~~
>>{: .source .language-fortran}
>>
>{: .solution}
>
{: .challenge}

### Optimisation

Now it's time to think about optimising the code.
Try it with a different numbers of ranks, up to
the maximum number you expect to use.
A parallel profiler is useful for finding the places 
where communication is taking time.

It's also good to check the speed with just one rank.
In most cases it can be made as fast as a serial code
using the same algorithm.

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
>> Runtime may increase if you need to use nodes that are
>> further away from each other in the network.
>>
>{: .solution}
{: .challenge}

{% include links.md %}

