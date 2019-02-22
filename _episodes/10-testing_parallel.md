---
title: "Testing a Parallel Code"
teaching: 20
exercises: 30
questions:
- "How do I make sure the program works?"
- "How parallel execution affect testing?"
objectives:
- "Learn to build test suits to verify a parallel program."
- "Point out differences between parallel and serial algorithms"
keypoints:
- "It's always easy to make mistakes"
- "Use tests to verify the parallel code against a serial code or known results"
---

## Automated Testing

Automated testing is very helpful when developing you application.
They allow you to quickly and easily check the correctness of your application
while making changes to it.
It's easy to make mistakes and debugging an entire application a significant
amout on time.
As a bonus, when the code is well covered by automated tests,
it will always work correctly, at least in the ways you have defined in the tests.

In the ideal scenario, you start from a correctly working serial application
with that alredy includes a full suite of tests for each function.
If this is not the case, it is best to start by writing tests for each function
in the files you with to parallellise.
If there are long functions, it may be a good idea to split them into smaller ones
and test these separately.

> ## Unit Tests and Integration Tests
>
> What we are doing here is called unit testing, testing individual small
> functions, or units.
> This is useful for developing from an already correct implementation of
> an algorithm.
> A failed test will immediately point out the location of a bug, and the
> smaller the units, the easiear it is to find the problem.
> Smaller units also allow each test to run faster.
> 
> You might also wish to test the correctness of the whole algorithm.
> This can be done in essentially the same way, but running essentially the entire
> program or a large section of it in the test.
> These integration tests are often separated from the unit tests
> and can take significantly longer to run.
>
> Especially in scientific applications, the correct output is often not known.
> In these cases you can test for known properties of the output, or test 
> with against a run of the serial version.
>
{: .callout}


> ## Unit testing in C
>
> In the following examples and challenges we use the Cmocka unit testing framework.
> Testing frameworks in general use similar concepts and any lessons learned here
> can be applied to any framework.
>
> Make sure you have the Cmocka library installed. Take a look at the [setup]({{ page.root }}{% link setup.md %}) for details.
> 
> Here's an simple example of a test suite with a single test:
>
>~~~
>#include <stdarg.h>
>#include <stddef.h>
>#include <setjmp.h>
>#include <cmocka.h>
>
>/* A test that checks if 1 is true */
>static void test_1_true(void **state) {
>   assert_true(1);
>}
>
>/* In the main function create the list of the tests */
>int main(void) {
>   const struct CMUnitTest tests[] = {
>      cmocka_unit_test(test_1_true),
>   };
>
>   // Call a library function that will run the tests
>   return cmocka_run_group_tests(tests, NULL, NULL);
>}
>~~~
> {: .output}
>
> Try compiling this using
> ~~~
> gcc file_name.c -lcmocka
> ~~~
> and running the resulting binary.
>
{: .prereq .foldable}


> ## Unit testing in Fortran
>
> In the following examples and challenges we use the FRUIT unit testing framework.
> Testing frameworks in general use similar concepts and any lessons learned here
> can be applied to any framework.
>
> You will need one file from the FRUIT zip.
> If you didnt donwload the it yet, take a look at the [setup]({{ page.root }}{% link setup.md %}) for details.
>
> Copy fruit.f90 from the src folder in the zip archive to your working directory.
> Compile it with
> ~~~
> gfortran fruit.f90
> ~~~
> {: .output}
>
> This creates two modules, fruit.mod and fruit_util.mod, and a object file, fruit.o.
> 
> Tests are defined in test modules. Here is a very simple example
> for making sure that true is true.
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
>     call assert_true( 1==1, "Boolean test")
>   end subroutine test_true
> end module example_test
>~~~
> {: .output}
> 
> Compile this module, too.
> 
> Next, we need to define a program to run. In addition to some FRUIT related calls,
> this will contain a list of your tests.
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
> {: .output}
>
> Compile this, including the example_test.o and fruit.o object files
> ~~~
> gfortran example_driver.f90 example_test.o fruit.o
> ~~~
> and run the resulting binary.
>
> The program will run our little test and produce some statistics about the tests overall.
>
{: .prereq .foldable}


## Test Suites Automate and Simplify Testing

A test suite is nothing but a program that runs your functions and checks that the output
is correct.
You check the output using a set of assert functions.
You assert that certain statements are true or that certain numbers are equal.
The test suite will report a failure if one or more of the assert statements fail.

It is good practice to write to name the test functions according to the function it is
testing and to give more information if there is more than one test for the function.
Don't be afraid of using long function names.
When the test suite reports a failure, it will report the name of the test function.
This way you know exactly where in your code the problem is.

> ## Tests
>
> This example code tests the find_sum function from a previous example.
>
>> ## Example in C
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
>>/* Test the find_sum function */
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
>>
>>int main(void) {
>>   const struct CMUnitTest tests[] = {
>>      cmocka_unit_test(test_find_sum),
>>   };
>>   return cmocka_run_group_tests(tests, NULL, NULL);
>>} 
>>~~~
>>{: .output}
>{: .prereq .foldable}
>
> Write a test for the find_maximum function as well and run both tests.
>
>> ## Solution in C
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
>>{: .output}
>{: .solution}
>
{: .challenge}

>## Setup and Teardown in Cmocka
>
> The groups in Cmocka exist simplify running multiple tests with similar inputs
> or environments.
> In solution to the exercise we added both tests to the same group and
> used cmocka_run_group_tests to run both tests.
> The tests share some setup, resulting in repeated code.
> Using a Setup and a Teardown function the solution would be
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
>/* Test the sum function */
>static void test_find_sum(void **state) {
>   double * vector = state[0];
>   int n_numbers = vector[0];
>   double sum;
>
>   //Find the sum and check it's correct
>   sum = find_sum( vector+1, n_numbers );
>   assert_true( sum == 523776 );
>}
>
>/* Test find_maximum */
>static void test_find_maximum(void **state) {
>   double * vector = state[0];
>   int n_numbers = vector[0];
>   double max;
>
>   //Find the sum and check it's correct
>   max = find_maximum( vector+1, n_numbers );
>   assert_true( max == 1023 );
>}
>
>
>static int setup(void **state)
>{
>   int n_numbers = 1024;
>   double * vector = malloc( (n_numbers+1)*sizeof(double) );
>   state[0] = (void *) vector;
>
>   // vector[0] will contain n_numbers
>   vector[0] = n_numbers;
>   // Generate a vector
>   for( int i=0; i<n_numbers; i++){
>      vector[1+i] = i;
>   }
>
>   return 0;
>}
>
>static int teardown(void **state)
>{
>   free( state[0] );
>   return 0;
>}
>
>int main(void) {
>   const struct CMUnitTest tests[] = {
>      cmocka_unit_test(test_find_sum),
>      cmocka_unit_test(test_find_maximum),
>   };
>   return cmocka_run_group_tests(tests, setup, teardown);
>}
>~~~
>{: .output}
>
> We probably added a couple of lines instead of saving any,
> but in a longer set of tests, this can save a lot of coding
> and allows you to change all of the tests at once.
>
{: .prereq .foldable}

You will notice that most test consist of three parts.
First they set up the inputs and the environment to mock how the function
might be called in the actual program.
They then run the function and finally do a set of checks on the output
and the environment.

## Testing an MPI Code

You can test an function that uses MPI in the same way.
The main differences are that you need to set up the MPI environment and
mock the data and the environment for each rank.

It's not good practice to include MPI_Init or MPI_Finalize in the test function itself.
If an assert is triggered, it may not be run, which could in turn ruin
the rest of the tests.
Instead we add MPI_Init and MPI_Finalize to the setup and teardown functions.
These will be run even if the test fails.

> ## Example in C
> ~~~
> #include <stdarg.h>
> #include <stddef.h>
> #include <setjmp.h>
> #include <cmocka.h>
> #include <stdio.h>
> #include <mpi.h>
> #include <stdlib.h>
> 
> // Calculate the sum of numbers in a vector
> double find_sum( double * vector, int N ){
>    double sum = 0;
>    double global_sum;
> 
>    // Calculate the sum on this rank as befor
>    for( int i=0; i<N; i++){
>       sum += vector[i];
>    }
> 
>    // Call MPI_Allreduce to find the full sum
>    MPI_Allreduce( &sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
> 
>    return global_sum;
> }
> 
> // Find the maximum of numbers in a vector
> double find_maximum( double * vector, int N ){
>    double max = 0;
>    double global_max;
> 
>    // Calculate the maximum on this rank as before
>    for( int i=0; i<N; i++){
>       if( vector[i] > max ){
>          max = vector[i];
>       }
>    }
> 
>    // Call MPI_Allreduce to find the maximum over all the ranks
>    MPI_Allreduce( &max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD );
> 
>    return global_max;
> }
> 
> 
> /* Test the find_maximum function */
> static void test_find_maximum(void **state) {
>    double * vector = state[0];
>    int n_numbers = vector[0];
>    double max;
>    
>    //Find the sum and check it's correct
>    max = find_maximum( vector+1, n_numbers );
>    assert_true( max == 1023 );
> }
> 
> 
> static int setup(void **state)
> {
>    int n_numbers = 1024;
>    double * vector;
>    
>    // Start by calling MPI_Init with mocked input
>    char *argv[] = {"mpi_test"};
>    int argc = 1;
>    MPI_Init(&argc, (char ***) &argv);
>    
>    vector = malloc( (n_numbers+1)*sizeof(double) );
>    state[0] = (void *) vector;
> 
>    // vector[0] will contain n_numbers
>    vector[0] = n_numbers;
>    // Generate a vector
>    for( int i=0; i<n_numbers; i++){
>       vector[1+i] = i;
>    }
> 
>    return 0;
> }
> 
> static int teardown(void **state)
> {
>    free( state[0] );
> 
>    // End by calling MPI_Finalize
>    MPI_Finalize();
> 
>    return 0;
> }
> 
> 
> int main(int argc, char** argv) {
>    int cmocka_return_code;
> 
>    const struct CMUnitTest tests[] = {
>       cmocka_unit_test(test_find_maximum),
>    };
>    
>    cmocka_return_code = cmocka_run_group_tests(tests, setup, teardown);
> 
>    return cmocka_return_code;
> }
> ~~~
> {: .output}
{: .callout .foldable}

> ## Testing MPI
>
> Implement a test of the find_sum function in the above example
>
{: .challenge}

Since all processes are running the tests, you will see multiple compies of the each of the
lines printed in the terminal.
The test suite is not really aware of MPI, it's just running multiple copies of the tests.
This is not necessarily a problem.
A test can fail on one rank and succeed on a different one.
In general you want to test all the ranks.

The "--tag-output" option for mpirun is usefull here.
It will append rank numbers to every line printed by the application.
This helps in keeping track of the ranks with errors.

Some tests might make sense only on a specific rank.
You can use if statements to run asserts only on specific ranks.
The code in a test is normal general purpose code.
It's good to keep them simple, though.

Another important caveat is the order of floating point operations.
Since the order of operations ofter depends on the number of ranks, and floating point operations
are not associative, different rounding errors can accumulate with different rank counts.
In most cases it's sufficient to allow some rounding error.
In sufficiently chaotic algorithms, it may not be possible to compare the results.
This does not necessarily mean that the implementation is incorrect.

> ## Floating Point Ordering
>
> Add a test for the product function in the example code.
>
{: .challenge}


{% include links.md %}

