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

## Going from Serial to Parallel

The examples used in the previous sections were perhaps not the most realistic.
In this section we will look at a more complete code and take it from serial to
parallel a couple of steps.
First, download the serial code for solving the Poisson equation in
[C]({{ page.root }}{% link code/poisson/poisson.c %})
or in
[Fortran]({{ page.root }}{% link code/poisson/poisson.f %})
and familiarise yourself with what how it works.

If you have your own serial code, it's advisable to choos a small
section of the code and go trough the same steps.

### Parallel regions

The first step is to identify parts of the code that
can be written in parallel.
Go trough the algorithm and decide for each region if the data can be partitioned for parallel execution
or if certain tasks can be separated and run in parallel.

Can you find large or time consuming serial regions?
The sum of the serial regions gives the minimum amount of time it will take to run the program.
If the serial parts are a significant part of the algorithm, it may not be possible to write an efficient parallel a version.
Can you replace the serial parts with a different, more parallel algorithm?


> ## Parallel Regions
>
> Identify any parallel and serial regions in the code.
> What would be the optimal parallellisation strategy?
>
>> ## Solution
>>
>> There are no clearly identifiable parallel tasks.
>> The loops over space can be run in parallel.
>> There are parallisable loops in
>> * the setup, when initialising the fields
>> * the calculation of the time step, unew
>> * the difference, unorm
>> * overwriting the field u
>> * writing the files could be done in parallel
>>
>> The best strategy would seem to be parallellising the loops.
>> This will lead to a data parallel implementation with the
>> elements of the fields u and rho divided across the ranks.
>>
>{: .solution}
>
{: .challenge}


### Communication Patterns

If you decide it's worth your time to try to parallelise the problem,
the next step is to decide how the ranks will share the tasks or the data.
This should be done for each region separately, but taking into account the time it would take to reorganise the data if you decide to change the pattern between regions.

The parallellisation strategy is often based on the
underlying problem the algorithm is dealing with.
For example in materials science, it makes sense
to decompose the data into domains by splitting the
physical volume of the material.

Most programs will use the same pattern in every region.
There is a cost to reorganising data, mostly having to do with communicating large blocks between ranks.
This is really only a problem if done in a tight loop, many times per second.


> ## Communications Patterns
>
> How would you divide the data to the ranks?
> When does each rank need data from the other
> ranks?
> Which ones?
>
>> ## Solution
>> Only one of the loops requires data from the other ranks,
>> and these are only nearest neighbours.
>>
>> Parallellising the loops would actually be the same thing as splitting the physical volume.
>> Each iteration of the loop accesses one element
>> in the rho and uner fields and four elements in
>> the u field.
>> The u field needs to be communicated if the value
>> is stored on a different rank.
>>
>> There is also a global reduction for calculating unorm.
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
>> The Poisson step fucntion could look like this:
>> ~~~
>>#include <stdio.h>
>>#include <math.h>
>>
>>#define MAX 100
>>
>>double poisson_step( 
>>    float u[MAX+2][MAX+2],
>>    float unew[MAX+2][MAX+2],
>>    float rho[MAX+2][MAX+2],
>>    float hsq
>>  ){
>>  double unorm;
>>
>>  // Calculate one timestep
>>  for( int j=1; j <= MAX; j++){
>>    for( int i=1; i <= MAX; i++){
>>        float difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
>>	    unew[i][j] =0.25*( difference - hsq*rho[i][j] );
>>    }
>>  }
>>
>>  // Find the difference compared to the previous time step
>>  unorm = 0.0;
>>  for( int j = 1;j <= MAX; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      float diff = unew[i][j]-u[i][j];
>>      unorm +=diff*diff;
>>    }
>>  }
>>  printf("unorm = %.8e\n",unorm);
>>
>>  // Overwrite u with the new field
>>  for( int j = 1;j <= MAX;j++){
>>    for( int i = 1;i <= MAX;i++){
>>      u[i][j] = unew[i][j];
>>    }
>>  }
>>
>>  return unorm;
>>}
>> ~~~
>> {:.output}
>>
>> Assuming the above is saved to poisson_step.c, the
>> the following wil run a simple test
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
>>   double unorm, residual;
>>
>>   /* Set variables */
>>   h = 0.1;
>>   hsq = h*h;
>>
>>   // Initialise the u and rho field to 0 
>>   for( int j=0; j <= MAX+1; j++ ){
>>      for( int i=0; i <= MAX+1; i++ ) {
>>         u[i][j] = 0.0;
>>         rho[i][j] = 0.0;
>>      }
>>   }
>>
>>   // Test a configuration with u=10 at x=1 and y=1
>>   u[1][1] = 10;
>>
>>   unorm = poisson_step( u, unew, rho, hsq );
>>
>>   assert_true( unorm == 112.5 );
>>}
>>
>>/* In the main function create the list of the tests */
>>int main(void) {
>>   const struct CMUnitTest tests[] = {
>>      cmocka_unit_test(test_poisson_step),
>>   };
>>
>>   // Call a library function that will run the tests
>>   return cmocka_run_group_tests(tests, NULL, NULL);
>>}
>> ~~~
>> {:.output}
>{: .solution}
>
{: .challenge}

### Write a Version for One Rank

Now write a version of the code that performs the
tasks of one node.
Communicate data in the simplest possible way,
probably just after it's created or just before it's needed.
Use blocking or non-blocking communication,
which ever you feel is simple. If you use non-blocking functions, call wait immediately.

The point is to write a simple code that works correctly.
You can optimise later.

> ## Parallel Execution
> 
> Let's parallellise split the outer loop across 
> the ranks.
> Write a program that permorms the iterations from
> j=rank*(MAX/n_ranks) to j=(rank+1)*(MAX/n_ranks).
> 
> Make sure the test works in parallel.
>
> It should not be necessary to do the nearest neighbour communication.
> You will need a reduction.
>
>> ## Solution in C
>>
>> In poisson_step_mpi.c:
>> ~~~
>> #include <stdio.h>
>> #include <math.h>
>> #include <mpi.h>
>> 
>> #define MAX 20
>> #define IMAX 1000
>> 
>> 
>> double poisson_step( 
>>    float u[][MAX+2],
>>    float unew[][MAX+2],
>>    float rho[][MAX+2],
>>    float hsq,
>>    int my_j_max
>>  ){
>>  double unorm, global_unorm;
>>
>>  // Calculate one timestep
>>  for( int j=1; j <= my_j_max; j++){
>>    for( int i=1; i <= MAX; i++){
>>        float difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
>>	    unew[i][j] =0.25*( difference - hsq*rho[i][j] );
>>    }
>>  }
>>
>>  // Find the difference compared to the previous time step
>>  unorm = 0.0;
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      float diff = unew[i][j]-u[i][j];
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
>>      u[i][j] = unew[i][j];
>>    }
>>  }
>>
>>  return global_unorm;
>>}
>>~~~
>>{: .output}
>>
>> The test file: 
>>~~~
>>#include <stdarg.h>
>>#include <stddef.h>
>>#include <setjmp.h>
>>#include <cmocka.h>
>>
>>#include "poisson_step_mpi.c"
>>
>>static void test_poisson_step(void **state) {
>>   float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
>>   float h, hsq;
>>   double unorm, residual;
>>   double diff;
>>   int rank, n_ranks, my_j_max;
>>
>>   /* Find the number of x-slices calculated by each rank */
>>   /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>   my_j_max = MAX/n_ranks;
>>
>>   /* Set variables */
>>   h = 0.1;
>>   hsq = h*h;
>>
>>   // Initialise the u and rho field to 0 
>>   for( int j=0; j <= my_j_max+1; j++ ){
>>      for( int i=0; i <= MAX+1; i++ ) {
>>         u[i][j] = 0.0;
>>         rho[i][j] = 0.0;
>>      }
>>   }
>>
>>   // Test a configuration with u=10 at x=1 and y=1
>>   // The actual x coordinate is my_j_max*rank + x
>>   // meaning that x=1 is on rank 0
>>   if( rank == 0 )
>>      u[1][1] = 10;
>>
>>   // Test one step
>>   unorm = poisson_step( u, unew, rho, hsq, my_j_max );
>>   assert_true( unorm == 112.5 );
>>
>>   // Test 50 steps
>>   // Expect this to fail
>>   for( int iteration=0; iteration<50; iteration++ ){
>>      unorm = poisson_step( u, unew, rho, hsq, my_j_max );
>>   }
>>   diff = unorm - 0.001838809444;
>>   assert_true( diff*diff < 1e-16 );
>>}
>>
>>/* In the main function create the list of the tests */
>>int main(int argc, char** argv) {
>>   int cmocka_return_value;
>>
>>   // First call MPI_Init
>>   MPI_Init(&argc, &argv);
>>
>>   const struct CMUnitTest tests[] = {
>>      cmocka_unit_test(test_poisson_step),
>>   };
>>
>>   // Call a library function that will run the tests
>>   cmocka_return_value = cmocka_run_group_tests(tests, NULL, NULL);
>>
>>   // Call finalize at the end
>>   MPI_Finalize();
>>
>>   return cmocka_return_value;
>>}
>>~~~
>>{: .output}
>{: .solution}
>
{: .challenge}

> ## Communication
> 
> Add in the nearest neighbour communication and test the result after
> 50 iterations.
>
>> ## Solution in C
>> Most rank needs to send its u[0] down to it's neighbour at rank-1 and
>> its u[my_j_max] up to rank+1.
>> There needs to be and exection for the rank 0 and the last rank.
>>
>> In poisson_step_mpi.c:
>>~~~
>>#include <stdio.h>
>>#include <math.h>
>>#include <mpi.h>
>>
>>#define MAX 20
>>
>>double poisson_step( 
>>    float u[][MAX+2],
>>    float unew[][MAX+2],
>>    float rho[][MAX+2],
>>    float hsq,
>>    int my_j_max,
>>    int rank,
>>    int n_ranks
>>  ){
>>  double unorm, global_unorm;
>>  float sendbuf[MAX],recvbuf[MAX];
>>  MPI_Status mpi_status;
>>
>>  // Calculate one timestep
>>  for( int j=1; j <= my_j_max; j++){
>>    for( int i=1; i <= MAX; i++){
>>        float difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
>>	    unew[i][j] =0.25*( difference - hsq*rho[i][j] );
>>    }
>>  }
>>
>>  // Find the difference compared to the previous time step
>>  unorm = 0.0;
>>  for( int j = 1;j <= my_j_max; j++){
>>    for( int i = 1;i <= MAX; i++){
>>      float diff = unew[i][j]-u[i][j];
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
>>      u[i][j] = unew[i][j];
>>    }
>>  }
>>
>>  // The u field has been changed, communicate it to neighbours
>>  // With blocking communication, half the ranks should send first
>>  // and the other half should receive first
>>  if ((rank%2) == 1) {
>>      // Ranks with odd number send first
>>
>>      // Send data down from rank to rank-1
>>      for( int i=0;i < MAX;i++) sendbuf[i] = unew[i+1][1];
>>      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD);
>>      // Receive dat from rank-1
>>      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD,&mpi_status);
>>      for( int i=0;i < MAX;i++) u[i+1][0] = recvbuf[i];
>>      
>>      if ( rank != (n_ranks-1)) {
>>        // Send data up to rank+1 (if I'm not the last rank)
>>	      for( int i=0;i < MAX;i++) sendbuf[i] = unew[i+1][my_j_max];
>>	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD);
>>	      // Receive data from rank+1
>>        MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD,&mpi_status);
>>	      for( int i=0;i < MAX;i++) u[i+1][my_j_max+1] = recvbuf[i];
>>      }
>>   
>>    } else {
>>      // Ranks with even number receive first
>>
>>      if (rank != 0) {
>>        // Receive data from rank-1 (if I'm not the first rank)
>>	      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD,&mpi_status);
>>	      for( int i=0;i < MAX;i++) u[i+1][0] = recvbuf[i];
>>	      
>>        // Send data down to rank-1
>>        for( int i=0;i < MAX;i++) sendbuf[i] = unew[i+1][1];
>>	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD);
>>      }
>>
>>      if (rank != (n_ranks-1)) {
>>        // Receive data from rank+1 (if I'm not the last rank)
>>	      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD,&mpi_status);
>>	      for( int i=0;i < MAX;i++) u[i+1][my_j_max+1] = recvbuf[i];
>>
>>        // Send data up to rank+1
>>	      for( int i=0;i < MAX;i++) sendbuf[i] = unew[i+1][my_j_max];
>>	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD);
>>      }
>>    }
>>
>>  return global_unorm;
>>}
>>~~~
>>
>> You also need to add rank and n_ranks to the function call in the test.
>>
>>{: .output}
>>
>{: .solution}
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
> There are no real problems with perfect strong scaling
> but some do get close.
>
> Write a main function that calls the poisson step 100 times and set MAX=512.
>
> Try running your program with an increasing number of ranks.
> Measure the time taken using the unix time utility.
> What are the limitations on scaling?
>
>> ## Solution 
>>
>> Exact numbers depend on the machine your running on, 
>> but with a small number of ranks, up to 4, you should
>> see the time decrease with the number of ranks.
>> At 8 ranks the result is a bit worse and doubling again to 16 has little effect.
>>
>> The figure below shows an example of the scaling with
>> MAX=512 and MAX=2048.
>> ![A figure showing the result described above for MAX=512 and MAX=2048]({{ page.root }}{% link fig/poisson_scaling_plot.png %})
>>
>> In the example, which runs on a 40 core Intel Scalable CPU,
>> using 32 ranks actually takes more time.
>> This is becouse the CPU has two sets of 20 cores that are
>> slightly separated.
>> The 32 ranks don't fit on one side and communicating between
>> the sides takes more time.
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
> Weak saling refers to increasing the size of the problem
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
> increasing MAX with the number of ranks.
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

