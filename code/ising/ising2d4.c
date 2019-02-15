/* serial code for ising model */

/* contact seyong.kim81@gmail.com for comments and questions */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N1 320
#define N1d2 160
#define N2 320

#define MAX N1*N2
#define MAXd2 N1d2*N2

/*  2d Ising model using Metropolis update algorithm*/

main(int argc, char** argv) {

  short int seed[3];
  int n,i,j,ij,i2,is,iup[MAX*2],idn[MAX*2],iter ;
  float beta,energy0,energy,deltae,esum,mag,env;
  float s[MAX],stmp;

  double esumt, magt;

  printf("beta = "); scanf("%f",&beta);
  printf("no. of iter = "); scanf("%d",&iter);

  seed[0]=13;seed[1]=35;  seed[2]=17;

  for(i = 0;i < MAX;i++) {
    if (erand48(seed) < 0.5)
      s[i] = 1.0; 
    else
      s[i] = -1.0;
  }

  for(j = 0;j < N2;j++)
    for(i = 0;i < N1;i++) {
      ij = i + j;
      i2 = i/2;
      is = i2 + j*N1d2;
      if(ij == (2*(ij/2))) {
	iup[is] = ((i+1)/2)%N1d2 + j*N1d2 + MAXd2;
	iup[is+MAX] = i2 + ((j+1)%N2)*N1d2 + MAXd2;
	idn[is] = ((i-1+N1)/2)%N1d2 + j*N1d2 + MAXd2;
	idn[is+MAX] = i2 + ((j-1+N2)%N2)*N1d2 + MAXd2;
      }
      else {
	iup[is+MAXd2] = ((i+1)/2)%N1d2 + j*N1d2;
	iup[is+MAXd2+MAX] = i2 + ((j+1)%N2)*N1d2;
	idn[is+MAXd2] = ((i-1+N1)/2)%N1d2 + j*N1d2;
	idn[is+MAXd2+MAX] = i2 + ((j-1+N2)%N2)*N1d2;
      }
    }

  /*  for(i = 0;i < MAX;i++) printf("i = %d, up-x = %d, dn-x = %d, up-y = %d, dn-y = %d\n",i,iup[i],idn[i],iup[i+MAX],idn[i+MAX]); */

  esumt = 0.0;
  magt = 0.0;
  for(n = 0;n < iter;n++) {
    esum = 0.0;
    mag = 0.0;
    for(i = 0;i < MAX;i++) {
      env = s[iup[i]]+s[iup[i+MAX]]+s[idn[i]]+s[idn[i+MAX]];
      energy0 = -beta*s[i]*env;
      stmp = -s[i];
      energy=-beta*stmp*env;
      deltae = energy0-energy;
      if(exp(deltae) > erand48(seed)) {
	s[i] = stmp;
	energy0 = energy;
      }
      mag = mag + s[i];
      esum = esum + energy0;
    }
    esum = esum/(MAX);
    mag = mag/(MAX);
    printf("average energy = %f, average magnetization = %f\n",esum,mag);
    esumt = esumt + esum;
    magt = magt + fabs(mag);
  }
  esumt = esumt/iter;
  magt = magt/iter;
  printf("over whole iterations \n average energy = %f, average magnetization = %f\n", esumt, magt);
}
