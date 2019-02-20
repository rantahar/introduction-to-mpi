#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MAX 1000

double poisson_step( 
    float u[][MAX+2],
    float unew[][MAX+2],
    float rho[][MAX+2],
    float hsq,
    int my_j_max,
    int rank,
    int n_ranks
  );

/* In the main function create the list of the tests */
int main(int argc, char** argv) {
   float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
   float h, hsq;
   double unorm, residual;
   double diff;
   int rank, n_ranks, my_j_max;

   // First call MPI_Init
   MPI_Init(&argc, &argv);

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

   // Start form a configuration with u=10 at x=1 and y=1
   // The actual x coordinate is my_j_max*rank + x
   // meaning that x=1 is on rank 0
   if( rank == 0 )
      u[1][1] = 10;

   // Test one step
   for( int iteration=0; iteration<1000; iteration++ ){
      unorm = poisson_step( u, unew, rho, hsq, my_j_max, rank, n_ranks );
   }

   // Call finalize at the end
   MPI_Finalize();

   return 0;
}