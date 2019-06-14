---
title: "Automated Testing"
teaching: 20
exercises: 30
questions:
- "How do I make sure the program works?"
- "How does parallel execution affect testing?"
objectives:
- "Learn to build test suites to verify a parallel program."
- "Point out differences between parallel and serial algorithms"
keypoints:
- "It's always easy to make mistakes"
- "Use tests to verify the parallel code against a serial code or known results"
---

## Automated Testing

Automated testing is very helpful when developing your application.
It allows you to quickly and easily check the correctness of your application
while making changes to it.
It's easy to make mistakes, and debugging an entire application takes a significant
amount of time.
As a bonus, when the code is well covered by automated tests,
it will always work correctly, at least in the ways you have defined in the tests.

In the ideal scenario, you start from a correctly working serial application
that already includes a full suite of tests for each function.
If this is not the case, it is best to start by writing tests for each function
in the files you wish to parallellise.
If there are long functions, it may be a good idea to split them into smaller ones
and test these separately.

> ## Unit Tests and Integration Tests
>
> What we are doing here is called unit testing, testing individual small
> functions, or units.
> This is useful for developing from an already correct implementation of
> an algorithm.
> A failed test will immediately point out the location of a bug, and the
> smaller the units, the easier it is to find the problem.
> Smaller units also allow each test to run faster.
> 
> You might also wish to test the correctness of the whole algorithm.
> This can be done in essentially the same way, but running the entire
> program or a large section of it in the test.
> These integration tests are often separated from the unit tests
> and can take significantly longer to run.
>
> Especially in scientific applications, the correct output is often not known.
> In these cases you can test for known properties of the output, or test 
> the results compared to a run of the serial version.
>
{: .callout}


## Unit testing

> In the following examples and challenges we use the Cmocka unit testing framework.
> Testing frameworks in general use similar concepts and any lessons learned here
> can be applied to any framework.
> 
> Make sure you have the Cmocka library installed. Take a look at the
> [setup page]({{ page.root }}{% link setup.md %}) for details.
> 
> Here's an simple example of a test suite with a single test:
> 
> ~~~
> #include <stdarg.h>
> #include <stddef.h>
> #include <setjmp.h>
> #include <cmocka.h>
> 
> /* A test that checks if 1 is true */
> static void test_1_true(void **state) {
>    assert_true(1);
> }
> 
> /* In the main function create the list of the tests */
> int main(void) {
>    const struct CMUnitTest tests[] = {
>       cmocka_unit_test(test_1_true),
>    };
> 
>    // Call a library function that will run the tests
>    return cmocka_run_group_tests(tests, NULL, NULL);
> }
> ~~~
> {: .source .language-c}
> 
> Try compiling this using
> ~~~
> gcc file_name.c -lcmocka
> ~~~
> {: .language-bash}
> and running the resulting binary.
> 
{: .noborder .show-c}


> In the following examples and challenges we use the FRUIT unit testing framework.
> Testing frameworks in general use similar concepts and any lessons learned here
> can be applied to any framework.
>
> You will need one file from the FRUIT ZIP archive.
> If you haven't downloaded it yet, take a look at the [setup]({{ page.root }}{% link setup.md %}) for details.
>
> Copy `fruit.f90` from the src folder in the ZIP archive to your working directory.
> Compile it with
> ~~~
> gfortran -c fruit.f90
> ~~~
>{: .language-bash}
>
> This creates two modules, `fruit.mod` and `fruit_util.mod`, and an object file, `fruit.o`.
> 
> Tests are defined in test modules. Here is a very simple example
> for making sure that 1 is 1.
>
>~~~
> module example_test
>   use fruit
>   implicit none
> 
> contains
>   subroutine test_true
>     integer:: result
> 
>     call assert_true( 1==1, "1 is 1")
>   end subroutine test_true
> end module example_test
>~~~
>{: .source .language-fortran}
> 
> We also need to define a program to run, called a driver.
> In addition to some FRUIT related calls, this will contain calls to all your tests.
>
>~~~
> program fruit_driver
>   use fruit
>   use example_test
>   call init_fruit
>   call test_true
>   call fruit_summary
>   call fruit_finalize
> end program fruit_driver
>~~~
>{: .source .language-fortran}
>
> Compile this and the test module, here in example_test.f08, and the fruit.o object file
> ~~~
> gfortran example_driver.f08 example_test.f08 fruit.o
> ~~~
>{: .language-bash}
> and run the resulting binary.
>
> The program will run our little test and produce some statistics about it.
>
{: .noborder .show-fortran}


## Test suites automate and simplify testing

A test suite is a program that runs your functions and checks that the output is correct.
You check the output using a set of assert functions.
You assert that certain statements are true or that certain numbers are equal.
The test suite will report a failure if one or more of the assert statements fail.

It is good practice to write to name the test functions according to the function it is
testing, and to give more information if there is more than one test for the function.
Don't be afraid of using long function names.
When the test suite reports a failure, it will report the name of the test function.
This way you know exactly where in your code the problem is.

> ## Tests
>
> The following example code tests the find_sum function from a previous example.
>
>~~~
>#include <stdarg.h>
>#include <stddef.h>
>#include <setjmp.h>
>#include <cmocka.h>
>#include <stdio.h>
>
>// Calculate the sum of numbers in a vector
>double find_sum( double * vector, int N ){
>   double sum = 0;
>   for( int i=0; i<N; i++){
>      sum += vector[i];
>   }
>   return sum;
>}
>
>// Find the maximum of numbers in a vector
>double find_maximum( double * vector, int N ){
>   double max = 0;
>   for( int i=0; i<N; i++){
>      if( vector[i] > max ){
>         max = vector[i];
>      }
>   }
>   return max;
>}
>
>
>/* Test the find_sum function */
>static void test_find_sum(void **state) {
>   int n_numbers = 1024;
>   double vector[n_numbers];
>   double sum;
>
>   // Generate a vector
>   for( int i=0; i<n_numbers; i++){
>      vector[i] = i;
>   }
>
>   //Find the sum and check it's correct
>   sum = find_sum( vector, n_numbers );
>   assert_true( sum == 523776 );
>}
>
>
>int main(void) {
>   const struct CMUnitTest tests[] = {
>      cmocka_unit_test(test_find_sum),
>   };
>   return cmocka_run_group_tests(tests, NULL, NULL);
>} 
>~~~
>{: .source .language-c .show-c}
>
>
>>Test module:
>>~~~
>>module example_test
>>  use fruit
>>  implicit none
>>
>>  contains
>>
>>    subroutine find_sum( vector, N, sum )
>>      real, intent(in) :: vector(:)
>>      real, intent(inout) :: sum
>>      integer, intent(in) :: N
>>      integer i
>>      
>>      sum = 0
>>      do i = 1, N
>>        sum = sum + vector(i)
>>      end do
>>
>>    end subroutine find_sum
>>
>>
>>    subroutine find_max( vector, N, max )
>>      real, intent(in) :: vector(:)
>>      real, intent(inout) :: max
>>      integer, intent(in) :: N
>>      integer i
>>      
>>      max = 0
>>      do i = 1, N
>>        if (max < vector(i)) then
>>          max = vector(i)
>>        end if
>>      end do
>>
>>    end subroutine find_max
>>
>>
>>    subroutine test_sum
>>      integer, parameter :: N=10
>>      integer i
>>      real vector(N)
>>      real sum
>>
>>      do i = 1, N
>>        vector(i) = i
>>      end do
>>  
>>      call find_sum( vector, N, sum )
>>
>>      call assert_true( sum == 55, "Test Sum")
>>    end subroutine test_sum
>>
>>end module example_test
>>~~~
>>{: .source .language-fortran}
>>
>>Driver Program:
>>~~~
>>program fruit_driver
>>  use fruit
>>  use example_test
>>  call init_fruit
>>  call test_sum
>>  call fruit_summary
>>  call fruit_finalize
>>end program fruit_driver
>>~~~
>>{: .source .language-fortran}
>{: .noborder .show-fortran}
>
>
>
> Write a test for the find_maximum function as well and run both tests.
>
>> ## Solution
>>~~~
>>#include <stdarg.h>
>>#include <stddef.h>
>>#include <setjmp.h>
>>#include <cmocka.h>
>>#include <stdio.h>
>>
>>// Calculate the sum of numbers in a vector
>>double find_sum( double * vector, int N ){
>>   double sum = 0;
>>   for( int i=0; i<N; i++){
>>      sum += vector[i];
>>   }
>>   return sum;
>>}
>>
>>// Find the maximum of numbers in a vector
>>double find_maximum( double * vector, int N ){
>>   double max = 0;
>>   for( int i=0; i<N; i++){
>>      if( vector[i] > max ){
>>         max = vector[i];
>>      }
>>   }
>>   return max;
>>}
>>
>>
>>/* Test the sum function */
>>static void test_find_sum(void **state) {
>>   int n_numbers = 1024;
>>   double vector[n_numbers];
>>   double sum;
>>
>>   // Generate a vector
>>   for( int i=0; i<n_numbers; i++){
>>      vector[i] = i;
>>   }
>>
>>   //Find the sum and check it's correct
>>   sum = find_sum( vector, n_numbers );
>>   assert_true( sum == 523776 );
>>}
>>
>>/* Test find_maximum */
>>static void test_find_maximum(void **state) {
>>   int n_numbers = 1024;
>>   double vector[n_numbers];
>>   double max;
>>
>>   // Generate a vector
>>   for( int i=0; i<n_numbers; i++){
>>      vector[i] = i;
>>   }
>>
>>   //Find the sum and check it's correct
>>   max = find_maximum( vector, n_numbers );
>>   assert_true( max == 1023 );
>>}
>>
>>
>>int main(void) {
>>   const struct CMUnitTest tests[] = {
>>      cmocka_unit_test(test_find_sum),
>>      cmocka_unit_test(test_find_maximum),
>>   };
>>   return cmocka_run_group_tests(tests, NULL, NULL);
>>}
>>~~~
>>{: .source .language-c}
>{: .solution .show-c}
>
>
>> ## Solution
>>
>> Add a second test subroutine to the test module
>>~~~
>>    subroutine test_max
>>      integer, parameter :: N=10
>>      integer i
>>      real vector(N)
>>      real max
>>
>>      do i = 1, N
>>        vector(i) = i
>>      end do
>>  
>>      call find_max( vector, N, max )
>>
>>      call assert_true( max == 10, "Test Max")
>>    end subroutine test_max
>>~~~
>>{: .source .language-fortran}
>>
>> And add the subroutine call to the driver program
>>~~~
>>program fruit_driver
>>  use fruit
>>  use example_test
>>  call init_fruit
>>  call test_sum
>>  call test_max
>>  call fruit_summary
>>  call fruit_finalize
>>end program fruit_driver
>>~~~
>>{: .source .language-fortran}
>{: .solution .show-fortran}
>
{: .challenge}


### Setup and teardown

Above we had to create a vector separately for both tests.
In a larger program, writing a test can require a large amount of setup.
The environment and the parameters need to mimic the way the function is
called in the code.

Doing these separately in each test results in a lot of repeated code.
This can be avoided using setup and teardown routines, run before and after
each test.

> The groups in Cmocka exist simplify running multiple tests with similar inputs
> or environments.
> In solution to the exercise we added both tests to the same group and
> used cmocka_run_group_tests to run both tests.
> 
> Test groups also share setup and teardown functions.
>
>~~~
>#include <stdarg.h>
>#include <stddef.h>
>#include <setjmp.h>
>#include <cmocka.h>
>#include <stdio.h>
>#include <stdlib.h>
>
>// Calculate the sum of numbers in a vector
>double find_sum( double * vector, int N ){
>  double sum = 0;
>  for( int i=0; i<N; i++){
>     sum += vector[i];
>  }
>  return sum;
>}
>
>// Find the maximum of numbers in a vector
>double find_maximum( double * vector, int N ){
>  double max = 0;
>  for( int i=0; i<N; i++){
>     if( vector[i] > max ){
>        max = vector[i];
>     }
>  }
>  return max;
>}
>
>
>/* Test the sum function */
>static void test_find_sum(void **state) {
>  double * state_vector = *state;
>  double * vector = state_vector +1;
>  int n_numbers = state_vector[0];
>  double sum;
>
>  //Find the sum and check it's correct
>  sum = find_sum( vector+1, n_numbers );
>  assert_true( sum == 523776 );
>}
>
>/* Test find_maximum */
>static void test_find_maximum(void **state) {
>  double * state_vector = *state;
>  double * vector = state_vector +1;
>  int n_numbers = state_vector[0];
>  double max;
>
>  //Find the sum and check it's correct
>  max = find_maximum( vector+1, n_numbers );
>  assert_true( max == 1023 );
>}
>
>
>static int setup(void **state)
>{
>  int n_numbers = 1024;
>  double * state_vector = malloc( (n_numbers+1)*sizeof(double) );
>  state[0] = (void *) state_vector;
>  double * vector = state_vector + 1;
>
>  // vector[0] will contain n_numbers
>  state_vector[0] = n_numbers;
>  
>  // Generate a vector
>  for( int i=0; i<n_numbers; i++){
>     vector[i] = i;
>  }
>
>  return 0;
>}
>
>static int teardown(void **state)
>{
>  free( state[0] );
>  return 0;
>}
>
>int main(void) {
>  const struct CMUnitTest tests[] = {
>     cmocka_unit_test(test_find_sum),
>     cmocka_unit_test(test_find_maximum),
>  };
>  return cmocka_run_group_tests(tests, setup, teardown);
>}
>~~~
>{: .source .language-c}
>
> We probably didn't save any lines of code by doing this.
> Actually we may have added some.
> But in a longer set of tests, this can save a lot of coding
> and allows you to change all of the tests at once.
>
{: .noborder .show-c}

>## Setup and Teardown in FRUIT
>
> Since FRUIT allows us to call the test functions directly, calling
> a setup and a teardown function is simple.
> First, let's add them to the test module and update the tests
> accordingly
>
>~~~
>    subroutine setup( vector, N )
>      real, intent(out) :: vector(:)
>      integer, intent(in) :: N
>      integer i
>
>      ! Set the vector
>      do i = 1, N
>        vector(i) = i
>      end do
>    end subroutine setup
>
>    subroutine teardown()
>      ! Nothing to do here
>    end subroutine teardown
>
>
>    subroutine test_sum(vector, N)
>      real, intent(in) :: vector(:)
>      integer, intent(in) :: N
>      integer i
>      real sum
>
>      call find_sum( vector, N, sum )
>
>      call assert_true( sum == 55, "Test Sum")
>    end subroutine test_sum
>
>
>    subroutine test_max(vector, N)
>      real, intent(in) :: vector(:)
>      integer, intent(in) :: N
>      integer i
>      real max
>
>      call find_max( vector, N, max )
>
>      call assert_true( max == 10, "Test Max")
>    end subroutine test_max
>~~~
>{: .source .language-fortran}
>
> We can then call them in the driver
>~~~
>program fruit_driver
>  use fruit
>  use example_test
>  implicit none
>
>  integer, parameter :: N=10
>  real vector(N)
>  call init_fruit
>
>  call setup(vector, N)
>  call test_sum(vector, N)
>  call teardown()
>
>  call setup(vector, N)
>  call test_max(vector, N)
>  call teardown()
>
>  call fruit_summary
>  call fruit_finalize
>end program fruit_driver
>~~~
>{: .source .language-fortran}
>
> In this case the tests do not change the vector, so there is no real need
> to rerun setup,
> but we might want to change this later, so it's good to be careful.
> In general, we don't want previous test to be able affect the current one.
>
> FRUIT also contains methods for automatically creating test groups, called
> baskets, and generating the driver to run them.
> These features use ruby, which you need to install separately.
> It's well worth looking into the features of this tool if you plan to
> develop in Fortran.
>
{: .noborder .show-fortran}

You will notice that most test consist of three parts.
First they set up the inputs and the environment to mock how the function
might be called in the actual program.
They then run the function and finally do a set of checks on the output
and the environment.

## Testing an MPI Code

You can test a function that uses MPI in the same way.
The main differences are that you need to set up the MPI environment and
mock the data and the environment for each rank.

It's not good practice to include `MPI_Init` or `MPI_Finalize` in the test function itself.
If an assert is triggered, `MPI_Finalize` may not be run, which could in turn ruin
the rest of the tests.
Instead call them either in the setup and teardown functions or in the main program.

~~~
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

// Calculate the sum of numbers in a vector
double find_sum( double * vector, int N ){
   double sum = 0;
   double global_sum;

   // Calculate the sum on this rank as befor
   for( int i=0; i<N; i++){
      sum += vector[i];
   }

   // Call MPI_Allreduce to find the full sum
   MPI_Allreduce( &sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

   return global_sum;
}

// Find the maximum of numbers in a vector
double find_maximum( double * vector, int N ){
   double max = 0;
   double global_max;

   // Calculate the maximum on this rank as before
   for( int i=0; i<N; i++){
      if( vector[i] > max ){
         max = vector[i];
      }
   }

   // Call MPI_Allreduce to find the maximum over all the ranks
   MPI_Allreduce( &max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD );

   return global_max;
}


/* Test the find_maximum function */
static void test_find_maximum(void **state) {
   double * state_vector, * vector;
   state_vector = state[0];
   vector = state_vector+1;
   int n_numbers = state_vector[0];
   double max;
   int n_ranks;

   //Find the sum and check it's correct
   max = find_maximum( vector+1, n_numbers );

   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

   assert_true( max == n_ranks*n_numbers - 1 );
}


static int setup(void **state)
{
   int n_numbers = 1024;
   int rank, my_first_number;
   double * state_vector, * vector;
   
   // Start by calling MPI_Init with mocked input
   char *argv[] = {"mpi_test"};
   int argc = 1;
   MPI_Init(&argc, (char ***) &argv);
   
   state_vector = malloc( (n_numbers+1)*sizeof(double) );
   state[0] = (void *) state_vector;
   vector = state_vector + 1;

   // vector[0] will contain n_numbers
   state_vector[0] = n_numbers;

   // Each rank will have n_numbers numbers,
   // starting from where the previous left
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   my_first_number = n_numbers*rank;

   // Generate a vector
   for( int i=0; i<n_numbers; i++){
      vector[i] = my_first_number + i;
   }

   return 0;
}

static int teardown(void **state)
{
   free( state[0] );

   // End by calling MPI_Finalize
   MPI_Finalize();

   return 0;
}


int main(int argc, char** argv) {
   int cmocka_return_code;

   const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_find_maximum),
   };
   
   cmocka_return_code = cmocka_run_group_tests(tests, setup, teardown);

   return cmocka_return_code;
}
 ~~~
{: .source .language-c .show-c}


> Test module:
> ~~~
>module example_test
>  use fruit
>  implicit none
>  include "mpif.h" 
>
>contains
>
>  subroutine find_sum( vector, N, global_sum )
>    real, intent(in) :: vector(:)
>    real, intent(inout) :: global_sum
>    real sum
>    integer, intent(in) :: N
>    integer i, ierr
>     
>    sum = 0
>    do i = 1, N
>      sum = sum + vector(i)
>    end do
>
>    ! Call MPI_Allreduce to find the full sum
>    call MPI_Allreduce( sum, global_sum, 1, MPI_REAL, MPI_SUM, MPI_COMM_WORLD, ierr )
>
>  end subroutine find_sum
>
>  ! Find the maximum of numbers in a vector
>  subroutine find_max( vector, N, global_max )
>    real, intent(in) :: vector(:)
>    real, intent(inout) :: global_max
>    real max
>    integer, intent(in) :: N
>    integer i, ierr
>     
>    max = 0
>    do i = 1, N
>       if (max < vector(i)) then
>          max = vector(i)
>       end if
>    end do
>
>    ! Call MPI_Allreduce to find the full sum
>    call MPI_Allreduce( max, global_max, 1, MPI_REAL, MPI_MAX, MPI_COMM_WORLD, ierr )
>
>  end subroutine find_max
>
>
>  ! Setup routine: call MPI_Init and create the vector
>  subroutine setup( vector, n_numbers )
>    real, intent(inout) :: vector(:)
>    integer, intent(in) :: n_numbers
>   
>    integer rank, ierr
>    real my_first_number
>    integer i
>
>    ! Get my rank
>    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>
>    ! Each rank will have n_numbers numbers,
>    ! starting from where the previous left off
>    my_first_number = n_numbers*rank;
>
>    ! Set the vector
>    do i = 1, n_numbers
>      vector(i) = my_first_number + i
>    end do
>  end subroutine setup
>
>  subroutine teardown()
>    ! Nothing to do here
>  end subroutine teardown
>
>
>  subroutine test_max(vector, N)
>    real, intent(in) :: vector(:)
>    integer, intent(in) :: N
>    integer i, n_ranks, ierr
>    real max
>
>    call find_max( vector, N, max )
>
>    ! Find the correct value using a simple serial method
>    call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>
>    call assert_true( max == n_ranks*N, "Test Max")
>  end subroutine test_max
>
>end module example_test
> ~~~
>{: .source .language-fortran}
>
>Driver Program:
>~~~
>program fruit_driver
>  use fruit
>  use example_test
>  implicit none
>
>  integer, parameter :: N=10
>  integer ierr
>  real vector(N)
>
>  call init_fruit
>
>  ! Start with MPI_Init
>  call MPI_Init(ierr)
>
>  call setup(vector, N)
>  call test_max(vector, N)
>  call teardown()
>
>  ! Call MPI_Finalize at the end
>  call MPI_Finalize(ierr)
>
>  call fruit_summary
>  call fruit_finalize
>end program fruit_driver
>~~~
>{: .source .language-fortran}
{: .noborder .show-fortran}

> ## Testing MPI
>
> Implement a test of the `find_sum` function in the above example
>
>
>> ## Solution
>> The test function:
>> ~~~
>>/* Test the find_sum_ function */
>>static void test_find_sum(void **state) {
>>   double * state_vector, * vector;
>>   state_vector = state[0];
>>   vector = state_vector+1;
>>   int n_numbers = state_vector[0];
>>   double sum, correct_sum;
>>   int n_ranks;
>>
>>   //Find the sum
>>   sum = find_sum( vector, n_numbers );
>>   
>>   //Find the correct value using a simple serial method
>>   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>   correct_sum = 0;
>>   for( int i=0; i<n_ranks*n_numbers; i++){
>>     correct_sum += i;
>>   }
>>   
>>   assert_true( sum == correct_sum );
>>}
>>~~~
>>{: .source .language-c}
>> 
>> Add the test to the list in the main function
>> ~~~
>>int main(int argc, char** argv) {
>>   int cmocka_return_code;
>>
>>   const struct CMUnitTest tests[] = {
>>      cmocka_unit_test(test_find_maximum),
>>      cmocka_unit_test(test_find_sum),
>>   };
>>   
>>   cmocka_return_code = cmocka_run_group_tests(tests, setup, teardown);
>>
>>   return cmocka_return_code;
>>}
>> ~~~
>>{: .source .language-c}
>{: .solution .show-c}
>
>
>> ## Solution
>>
>>Add the test function to the test module
>>~~~
>>  subroutine test_max(vector, N)
>>    real, intent(in) :: vector(:)
>>    integer, intent(in) :: N
>>    integer i
>>    real max
>>
>>    call find_max( vector, N, max )
>>
>>    write(6,*) "Maximum = ", max
>>
>>    call assert_true( max == 20, "Test Max")
>>  end subroutine test_max
>>~~~
>>{: .source .language-fortran}
>>
>>Driver Program:
>>~~~
>>program fruit_driver
>>  use fruit
>>  use example_test
>>  implicit none
>>
>>  integer, parameter :: N=10
>>  integer ierr
>>  real vector(N)
>>
>>  call init_fruit
>>
>>  ! Start with MPI_Init
>>  call MPI_Init(ierr)
>>
>>  call setup(vector, N)
>>  call test_max(vector, N)
>>  call teardown()
>>
>>  call setup(vector, N)
>>  call test_sum(vector, N)
>>  call teardown()
>>
>>  ! Call MPI_Finalize at the end
>>  call MPI_Finalize(ierr)
>>
>>  call fruit_summary
>>  call fruit_finalize
>>end program fruit_driver
>>~~~
>>{: .source .language-fortran}
>{: .solution .show-fortran}
{: .challenge}

Since all processes are running the tests, you will see multiple copies of the each of the
lines printed in the terminal.
The test suite is not aware of MPI---it's just running multiple copies of the tests.
This is not necessarily a problem.
A test can fail on one rank and succeed on a different one.
In general you want to test all the ranks.

The `--tag-output` option for `mpirun` is useful here.
It will append rank numbers to every line printed by the application.
This helps in keeping track of the ranks with errors.

Some tests might make sense only on a specific rank.
You can use if statements to run asserts only on that ranks.
The code in a test is normal general purpose code.
It's good to keep them simple, though.

Another important caveat is the order of floating point operations.
Since the order of operations ofter depends on the number of ranks, and floating point operations
are not associative, different rounding errors can accumulate with different rank counts.
In most cases it's sufficient to allow some rounding error.
In very chaotic algorithms, it may not be possible to compare the results.
This does not necessarily mean that the implementation is incorrect.

> ## Floating Point Ordering
>
> Set the vector lenght to 2014 in the above test and increase the number of ranks.
> At some point the test of the `find_sum` function will start to fail.
> * Why does it fail? What is the difference between the serial and the MPI versions?
> * Is this a problem? How would you fix it?
> * Let's say we don't care about the small difference.
> Change the test to only report differences larger than 0.0001.
>
{: .challenge}


{% include links.md %}

