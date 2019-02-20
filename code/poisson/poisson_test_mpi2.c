#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "poisson_step_mpi2.c"

static void test_poisson_step(void **state) {
   float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
   float h, hsq;
   double unorm, residual;
   double diff;
   int rank, n_ranks, my_j_max;

   /* Find the number of x-slices calculated by each rank */
   /* The simple calculation here assumes that MAX is divisible by n_ranks */
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
   my_j_max = MAX/n_ranks;

   /* Set variables */
   h = 0.1;
   hsq = h*h;

   // Initialise the u and rho field to 0 
   for( int j=0; j <= my_j_max+1; j++ ){
      for( int i=0; i <= MAX+1; i++ ) {
         u[i][j] = 0.0;
         rho[i][j] = 0.0;
      }
   }

   // Test a configuration with u=10 at x=1 and y=1
   // The actual x coordinate is my_j_max*rank + x
   // meaning that x=1 is on rank 0
   if( rank == 0 )
      u[1][1] = 10;

   // Test one step
   unorm = poisson_step( u, unew, rho, hsq, my_j_max, rank, n_ranks );
   assert_true( unorm == 112.5 );

   // Test 50 steps
   // Expect this to fail
   for( int iteration=0; iteration<50; iteration++ ){
      unorm = poisson_step( u, unew, rho, hsq, my_j_max, rank, n_ranks );
   }
   diff = unorm - 0.001838809444;
   assert_true( diff*diff < 1e-16 );
}

/* In the main function create the list of the tests */
int main(int argc, char** argv) {
   int cmocka_return_value;

   // First call MPI_Init
   MPI_Init(&argc, &argv);

   const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_poisson_step),
   };

   // Call a library function that will run the tests
   cmocka_return_value = cmocka_run_group_tests(tests, NULL, NULL);

   // Call finalize at the end
   MPI_Finalize();

   return cmocka_return_value;
}