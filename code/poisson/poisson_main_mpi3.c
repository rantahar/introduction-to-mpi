#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MAX 128

double poisson_step( 
    float **u,
    float **unew,
    float **rho,
    float hsq,
    int my_j_max,
    int rank,
    int n_ranks
  );

/* In the main function create the list of the tests */
int main(int argc, char** argv) {
   float **u, **unew, **rho;
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

   /* Reserve memory for the fields */
   u = malloc( (my_j_max+2)*sizeof(float*) );
   unew = malloc( (my_j_max+2)*sizeof(float*) );
   rho = malloc( (my_j_max+2)*sizeof(float*) );

   for( int i=0; i<my_j_max+2; i++){
      u[i] = malloc( (MAX+2)*sizeof(float) );
      unew[i] = malloc( (MAX+2)*sizeof(float) );
      rho[i] = malloc( (MAX+2)*sizeof(float) );
   }

   /* Set variables */
   h = 0.1;
   hsq = h*h;

   // Initialise the u and rho field to 0 
   for( int j=0; j <= my_j_max+1; j++ ){
      for( int i=0; i <= MAX+1; i++ ) {
         u[j][i] = 0.0;
         rho[j][i] = 0.0;
      }
   }

   // Start form a configuration with u=10 at x=1 and y=1
   // The actual x coordinate is my_j_max*rank + x
   // meaning that x=1 is on rank 0
   if( rank == 0 )
      u[1][1] = 10;

   // Test one step
   for( int iteration=0; iteration<10; iteration++ ){
      unorm = poisson_step( u, unew, rho, hsq, my_j_max, rank, n_ranks );
   }

   if( rank == 0 ){
      printf("Final unorm = %f\n", unorm);
   }

   // Free memory and finalize
   for( int i=0; i<my_j_max+2; i++){
      free(u[i]);
      free(unew[i]);
      free(rho[i]);
   }

   free(u);
   free(unew);
   free(rho);

   // Call finalize at the end
   MPI_Finalize();

   return 0;
}