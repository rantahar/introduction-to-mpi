/* a serial code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MAX 20
#define IMAX 1000


double poisson_step( 
    float u[][MAX+2],
    float unew[][MAX+2],
    float rho[][MAX+2],
    float hsq,
    int my_j_max
  ){
  double unorm, global_unorm;

  // Calculate one timestep
  for( int j=1; j <= my_j_max; j++){
    for( int i=1; i <= MAX; i++){
        float difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
	    unew[i][j] =0.25*( difference - hsq*rho[i][j] );
    }
  }

  // Find the difference compared to the previous time step
  unorm = 0.0;
  for( int j = 1;j <= my_j_max; j++){
    for( int i = 1;i <= MAX; i++){
      float diff = unew[i][j]-u[i][j];
      unorm +=diff*diff;
    }
  }

  // Use Allreduce to calculate the sum over ranks
  MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

  // Overwrite u with the new field
  for( int j = 1;j <= my_j_max; j++){
    for( int i = 1;i <= MAX; i++){
      u[i][j] = unew[i][j];
    }
  }

  return global_unorm;
}
