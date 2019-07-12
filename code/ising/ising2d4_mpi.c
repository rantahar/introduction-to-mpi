/* parallel code for ising model */

/* contact seyong.kim81@gmail.com for comments and questions */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <mpi.h>

#define N1 320
#define N1d2 (N1/2)
#define N2 320

#define VOLUME N1*N2
#define VOLUMEd2 N1*N2

/*  2d Ising model using Metropolis update algorithm
    periodic boundary condition for x- and y-direction
    version 3 : checker board partitioned
    MPI version
    assumes that 1-D ring topology in the y direction and no of ranks is
    a divisor of N2 and N2/n_ranks is an even integer for convenience */

int main(int argc, char** argv) {

  short int seed[3];
  int n,i,j,is,ii,itag,iter;
  int xup[VOLUME], yup[VOLUME], xdn[VOLUME], ydn[VOLUME];
  int rank, n_ranks, nextup, nextdn, iroot, subN2, subVOLUME, subVOLUMEd2;
  float s[VOLUME], stmp, sendbuf[N1d2], recvbuf[N1d2];
  float beta, new_energy, energy_now, deltae;

  double esumt,magt,esum,mag,esumsub,magsub;

  MPI_Status status;
  MPI_Request request;

  /* Initialize MPI and set rank parameters */
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
  iroot = 0;
  subN2 = N2/n_ranks;
  subVOLUME = VOLUME/n_ranks;
  subVOLUMEd2 = subVOLUME/2;
  nextup = (rank+1)%n_ranks; nextdn = (rank-1+n_ranks)%n_ranks;

  /* Read parameters. Beta is the inverse of the temperature. */
  if(rank == 0){
    FILE *fp = fopen("parameter","r");
    fscanf(fp,"beta %f\n", &beta);
    fscanf(fp,"iter %d\n", &iter);
    fclose(fp);
    printf("Beta = %f\n", beta);
    printf("Iter = %d\n", iter);
  }

  /* Broadcast parameters to all ranks */
  MPI_Bcast( &beta, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast( &iter, 1, MPI_INT, 0, MPI_COMM_WORLD);

  /* Set random seed for erand. Needs to be different for each rank */
  seed[0]=12+rank; seed[1]=35+rank; seed[2]=17+rank;

  /* Initialize each point randomly */
  for(i = 0;i < subVOLUME;i++) {
    if (erand48(seed) < 0.5)
      s[i] = 1.0; 
    else
      s[i] = -1.0;
  }

  /* Create and index of neighbours
     The sites are partitioned to even an odd,
     with even sites first in the array */
  for(j = 0;j < subN2;j++) for(i = 0;i < N1;i++) {
    int ij = i + j;
    int i2 = i/2;
    int is = i2 + j*N1d2;
    if(ij == (2*(ij/2))) {
	    xup[is] = ((i+1)/2)%N1d2 + j*N1d2 + subVOLUMEd2;
	    yup[is] = i2 + ((j+1)%subN2)*N1d2 + subVOLUMEd2;
	    xdn[is] = ((i-1+N1*2)/2)%N1d2 + j*N1d2 + subVOLUMEd2;
	    ydn[is] = i2 + ((j-1+N2)%subN2)*N1d2 + subVOLUMEd2;
    }
    else {
	    xup[is+subVOLUMEd2] = ((i+1)/2)%N1d2 + j*N1d2;
	    yup[is+subVOLUMEd2] = i2 + ((j+1)%subN2)*N1d2;
	    xdn[is+subVOLUMEd2] = ((i-1+N1)/2)%N1d2 + j*N1d2;
	    ydn[is+subVOLUMEd2] = i2 + ((j-1+N2)%subN2)*N1d2;
    }
  }

  /* Initialize the measurements */
  esumt = 0.0;
  magt = 0.0;

  /* Run a number of iterations */
  for(n = 0;n < iter;n++) {
    esum = 0.0;
    mag = 0.0;
    esumsub = 0.0;
    magsub = 0.0;

    /* Do for even and odd sites */
    for(int parity=0; parity<2; parity++) {
      int other_parity = 1 - parity;
      
      /* Communicate and update the j=0 boundary */
      itag = 11;
      ii = parity*subVOLUMEd2;
      for(i = 0;i < N1d2;i++) {
	      sendbuf[i] = s[i+subVOLUMEd2-N1d2+other_parity*subVOLUMEd2];
      }
      MPI_Irecv(&recvbuf,N1d2,MPI_FLOAT,nextdn,itag,MPI_COMM_WORLD,&request);
      MPI_Send(&sendbuf,N1d2,MPI_FLOAT,nextup,itag,MPI_COMM_WORLD);
      MPI_Wait(&request,&status);

      for(i=0; i<N1d2; i++){
        float neighbours = s[xup[i+ii]] + s[yup[i+ii]]
	                     + s[xdn[i+ii]] + recvbuf[i];
	      stmp = -s[i+ii];
        energy_now = -s[i+ii]*neighbours;
        new_energy = -stmp*neighbours;
        deltae = new_energy-energy_now;
	      if(exp(-beta*deltae) > erand48(seed)) {
	        s[i+ii] = stmp;
	        energy_now = new_energy;
	      }
	      magsub = magsub + s[i+ii];
	      esumsub = esumsub + energy_now;
      }

      /* Update the bulk of the lattice, everything but the boundaries */
      for(j = 1;j < (subN2-1);j++) 
	      for(i = 0;i < N1d2;i++) {
	        is = i + N1d2*j + ii;
	        float neighbours = s[xup[is]] + s[yup[is]]
                         + s[xdn[is]] + s[ydn[is]];
	        stmp = -s[is];
          energy_now = -s[is]*neighbours;
          new_energy = -stmp*neighbours;
          deltae = new_energy-energy_now;
	        if(exp(-beta*deltae) > erand48(seed)) {
	          s[is] = stmp;
	          energy_now = new_energy;
	        }
	        magsub = magsub + s[is];
	        esumsub = esumsub + energy_now;
	      }
      
      /* Update the j = subN2-1 boundary */
      itag = 22;
      for(i = 0;i < N1d2;i++) {
	      sendbuf[i] = s[i+other_parity*subVOLUMEd2];
      }
      MPI_Irecv(&recvbuf,N1d2,MPI_FLOAT,nextup,itag,MPI_COMM_WORLD,&request);
      MPI_Send(&sendbuf,N1d2,MPI_FLOAT,nextdn,itag,MPI_COMM_WORLD);
      MPI_Wait(&request,&status);

      ii = ii+subVOLUMEd2-N1d2;
      for(i = 0;i < N1d2;i++) {
	      float neighbours = s[xup[i+ii]] + recvbuf[i]
	                     + s[xdn[i+ii]] + s[ydn[i+ii]];
	      stmp = -s[i+ii];
        energy_now = -s[i+ii]*neighbours;
        new_energy = -stmp*neighbours;
        deltae = new_energy-energy_now;
	      if(exp(-beta*deltae) > erand48(seed)) {
	        s[i+ii] = stmp;
	        energy_now = new_energy;
	      }
	      magsub = magsub + s[i+ii];
	      esumsub = esumsub + energy_now;
      }

      /* Sum the energy and magnetisation over the ranks */
      MPI_Reduce(&esumsub,&esum,1,MPI_DOUBLE,MPI_SUM,iroot,MPI_COMM_WORLD);
      MPI_Reduce(&magsub,&mag,1,MPI_DOUBLE,MPI_SUM,iroot,MPI_COMM_WORLD);
    }

    /* Calculate average measurements and print */
    if(rank == 0) {
      esum = esum/(VOLUME);
      mag = mag/(VOLUME);
      esumt = esumt + esum;
      magt = magt + fabs(mag);

      printf("average energy = %f, average magnetization = %f\n",esum,mag);
    }
  }

  esumt = esumt/iter;
  magt = magt/iter;
  if(rank == 0){
    printf("Over the whole simulation:\n");
    printf("average energy = %f, average magnetization = %f\n", esumt, magt);
  }

  return MPI_Finalize();
}
