/* a serial code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>
#include <mpi.h>

#include "scorep/SCOREP_User.h"

#define MAX 1024


double poisson_step( 
    float **u,
    float **unew,
    float **rho,
    float hsq,
    int my_j_max,
    int rank,
    int n_ranks
  ){
  double unorm, global_unorm;
  float sendbuf[MAX],recvbuf[MAX];
  MPI_Status mpi_status;

  /* Define the Scorep region */
  SCOREP_USER_REGION_DEFINE( norm );

  // Calculate one timestep
  for( int j=1; j <= my_j_max; j++){
    for( int i=1; i <= MAX; i++){
        float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
	    unew[j][i] =0.25*( difference - hsq*rho[j][i] );
    }
  }

  SCOREP_USER_REGION_BEGIN( norm, "<norm>", SCOREP_USER_REGION_TYPE_LOOP );
  // Find the difference compared to the previous time step
  unorm = 0.0;
  for( int j = 1;j <= my_j_max; j++){
    for( int i = 1;i <= MAX; i++){
      float diff = unew[j][i]-u[j][i];
      unorm +=diff*diff;
    }
  }

  // Use Allreduce to calculate the sum over ranks
  MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

  SCOREP_USER_REGION_END( norm );

  // Overwrite u with the new field
  for( int j = 1;j <= my_j_max; j++){
    for( int i = 1;i <= MAX; i++){
      u[j][i] = unew[j][i];
    }
  }

  // The u field has been changed, communicate it to neighbours
  // With blocking communication, half the ranks should send first
  // and the other half should receive first
  if ((rank%2) == 1) {
      // Ranks with odd number send first

      // Send data down from rank to rank-1
      for( int i=0;i < MAX;i++) sendbuf[i] = unew[1][i+1];
      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD);
      // Receive dat from rank-1
      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD,&mpi_status);
      for( int i=0;i < MAX;i++) u[0][i+1] = recvbuf[i];
      
      if ( rank != (n_ranks-1)) {
        // Send data up to rank+1 (if I'm not the last rank)
	      for( int i=0;i < MAX;i++) sendbuf[i] = unew[my_j_max][i+1];
	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD);
	      // Receive data from rank+1
        MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD,&mpi_status);
	      for( int i=0;i < MAX;i++) u[my_j_max+1][i+1] = recvbuf[i];
      }
   
    } else {
      // Ranks with even number receive first

      if (rank != 0) {
        // Receive data from rank-1 (if I'm not the first rank)
	      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD,&mpi_status);
	      for( int i=0;i < MAX;i++) u[0][i+1] = recvbuf[i];
	      
        // Send data down to rank-1
        for( int i=0;i < MAX;i++) sendbuf[i] = unew[1][i+1];
	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD);
      }

      if (rank != (n_ranks-1)) {
        // Receive data from rank+1 (if I'm not the last rank)
	      MPI_Recv(recvbuf,MAX,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD,&mpi_status);
	      for( int i=0;i < MAX;i++) u[my_j_max+1][i+1] = recvbuf[i];

        // Send data up to rank+1
	      for( int i=0;i < MAX;i++) sendbuf[i] = unew[my_j_max][i+1];
	      MPI_Send(sendbuf,MAX,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD);
      }
    }

  return global_unorm;
}
