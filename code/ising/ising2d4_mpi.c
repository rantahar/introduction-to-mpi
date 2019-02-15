/* parallel code for ising model */

/* contact seyong.kim81@gmail.com for comments and questions */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <mpi.h>

#define N1 160
#define N2 320

#define MAX N1*N2*2
#define MAXd2 N1*N2

/*  2d Ising model using Metropolis update algorithm
    periodic boundary condition for x- and y-direction
    version 3 : checker board partitioned
    MPI version
    assumes that 1-D ring topology in the y direction and no of nodes is
    a divisor of N2 and N2/numtask is an even integer for convenience */

main(int argc, char** argv) {

  short int seed[3];
  int n,i,j,ij,i2,is,ii,ieo,ioe,itag,iup[MAX*2],idn[MAX*2],iter;
  int node,numtask,nextup,nextdn,iroot,subN2,subMAX,subMAXd2;
  float beta,energy0,energy,deltae,env;
  float s[MAX],stmp,sendbuf[N1],recvbuf[N1];

  double esumt,magt,esum,mag,esumsub,magsub;

  MPI_Status status;
  MPI_Request request;

  FILE *fp, *fopen();

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&node);
  MPI_Comm_size(MPI_COMM_WORLD,&numtask);

  fp = fopen("fort.12","r");
  fscanf(fp,"%f %d",&beta,&iter);

    /*  printf("beta = "); scanf("%f",&beta);
	printf("no. of iter = "); scanf("%d",&iter); */

  seed[0]=13+node; seed[1]=35+node; seed[2]=17+node;

  iroot = 0;
  subN2 = N2/numtask;
  subMAX = MAX/numtask;
  subMAXd2 = subMAX/2;

  nextup = (node+1)%numtask; nextdn = (node-1+numtask)%numtask;

  /*  printf("node = %d, %d, %d \n",node,nextup,nextdn);*/

  for(i = 0;i < subMAX;i++) {
    if (erand48(seed) < 0.5)
      s[i] = 1.0; 
    else
      s[i] = -1.0;
  }

  for(j = 0;j < subN2;j++)
    for(i = 0;i < N1*2;i++) {
      ij = i + j;
      i2 = i/2;
      is = i2 + j*N1;
      if(ij == (2*(ij/2))) {
	iup[is] = ((i+1)/2)%N1 + j*N1 + subMAXd2;
	iup[is+subMAX] = i2 + ((j+1)%subN2)*N1 + subMAXd2;
	idn[is] = ((i-1+N1*2)/2)%N1 + j*N1 + subMAXd2;
	idn[is+subMAX] = i2 + ((j-1+N2)%subN2)*N1 + subMAXd2;
      }
      else {
	iup[is+subMAXd2] = ((i+1)/2)%N1 + j*N1;
	iup[is+subMAXd2+subMAX] = i2 + ((j+1)%subN2)*N1;
	idn[is+subMAXd2] = ((i-1+N1*2)/2)%N1 + j*N1;
	idn[is+subMAXd2+subMAX] = i2 + ((j-1+N2)%subN2)*N1;
      }
    }

  /*  for(i = 0;i < MAX;i++) printf("i = %d, up-x = %d, dn-x = %d, up-y = %d, dn-y = %d\n",i,iup[i],idn[i],iup[i+MAX],idn[i+MAX]); */

  esumt = 0.0;
  magt = 0.0;
  for(n = 0;n < iter;n++) {

    esum = 0.0;
    mag = 0.0;
    esumsub = 0.0;
    magsub = 0.0;

    for(ieo = 0;ieo < 2;ieo++) {
      ioe = 1 - ieo;

      /* j = 0 boundary */

      itag = 11;
      ii = ieo*subMAXd2;
      for(i = 0;i < N1;i++) {
	sendbuf[i] = s[i+subMAXd2-N1+ioe*subMAXd2];
      }
      MPI_Irecv(&recvbuf,N1,MPI_FLOAT,nextdn,itag,MPI_COMM_WORLD,&request);

      MPI_Send(&sendbuf,N1,MPI_FLOAT,nextup,itag,MPI_COMM_WORLD);

      MPI_Wait(&request,&status);

      for(i = 0;i < N1;i++) {
	env = s[iup[i+ii]]+s[iup[i+ii+subMAX]]
	     +s[idn[i+ii]]+recvbuf[i];
	energy0 = -beta*s[i+ii]*env;
	stmp = -s[i+ii];
	energy=-beta*stmp*env;
	deltae = energy0-energy;
	if(exp(deltae) > erand48(seed)) {
	  s[i+ii] = stmp;
	  energy0 = energy;
	}
	magsub = magsub + s[i+ii];
	esumsub = esumsub + energy0;
      }

      for(j = 1;j < (subN2-1);j++) 
	for(i = 0;i < N1;i++) {
	  is = i + N1*j + ii;
	  env = s[iup[is]]+s[iup[is+subMAX]]
	       +s[idn[is]]+s[idn[is+subMAX]];
	  energy0 = -beta*s[is]*env;
	  stmp = -s[is];
	  energy=-beta*stmp*env;
	  deltae = energy0-energy;
	  if(exp(deltae) > erand48(seed)) {
	    s[is] = stmp;
	    energy0 = energy;
	  }
	  magsub = magsub + s[is];
	  esumsub = esumsub + energy0;
	}
      
      /* j = subN2-1 boundary */

      itag = 22;
      for(i = 0;i < N1;i++) {
	sendbuf[i] = s[i+ioe*subMAXd2];
      }
      MPI_Irecv(&recvbuf,N1,MPI_FLOAT,nextup,itag,MPI_COMM_WORLD,&request);

      MPI_Send(&sendbuf,N1,MPI_FLOAT,nextdn,itag,MPI_COMM_WORLD);

      MPI_Wait(&request,&status);

      ii = ii+subMAXd2-N1;
      for(i = 0;i < N1;i++) {
	env = s[iup[i+ii]]+recvbuf[i]
	     +s[idn[i+ii]]+s[idn[i+ii+subMAX]];
	energy0 = -beta*s[i+ii]*env;
	stmp = -s[i+ii];
	energy=-beta*stmp*env;
	deltae = energy0-energy;
	if(exp(deltae) > erand48(seed)) {
	  s[i+ii] = stmp;
	  energy0 = energy;
	}
	magsub = magsub + s[i+ii];
	esumsub = esumsub + energy0;
      }
      MPI_Reduce(&esumsub,&esum,1,MPI_DOUBLE,MPI_SUM,iroot,MPI_COMM_WORLD);
      MPI_Reduce(&magsub,&mag,1,MPI_DOUBLE,MPI_SUM,iroot,MPI_COMM_WORLD);
    }
    if(node == 0) {
      esum = esum/(MAX);
      mag = mag/(MAX);
      printf("average energy = %f, average magnetization = %f\n",esum,mag);
      esumt = esumt + esum;
      magt = magt + fabs(mag);
    }
  }
  esumt = esumt/iter;
  magt = magt/iter;
  if(node == 0)
  printf("over whole iterations \n average energy = %f, average magnetization = %f\n", esumt, magt);

  MPI_Finalize();

}
