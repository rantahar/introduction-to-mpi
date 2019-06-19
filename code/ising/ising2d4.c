/* serial code for ising model */

/* contact seyong.kim81@gmail.com for comments and questions */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N1 320
#define N1d2 (N1/2)
#define N2 320

#define VOLUME N1*N2
#define VOLUMEd2 N1d2*N2

/* 2d Ising model using Metropolis update algorithm */

int main(int argc, char** argv) {

  short int seed[3];
  int n,i,j,iter;
  int xup[VOLUME], yup[VOLUME], xdn[VOLUME], ydn[VOLUME];
  float beta,esum,mag;
  int s[VOLUME];

  double esumt, magt;

  /* Ask for parameters. Beta is the inverse of the temperature */
  fp = fopen("parameter","r");
  fscanf(fp,"beta %f %d",&beta);
  fscanf(fp,"iter %d",&iter);
  fclose(fp);

  /* Set random seed for erand */
  seed[0]=13; seed[1]=35; seed[2]=17;

  /* Initialize each point randomly */
  for(i = 0;i < VOLUME;i++) {
    if(erand48(seed) < 0.5){
      s[i] = 1.0; 
    }
    else {
      s[i] = -1.0;
    }
  }

  /* Create and index of neighbours
     The sites are partitioned to even an odd,
     with even sites first in the array */
  for( j=0; j<N2; j++) for( i=0; i<N1; i++) {
    int i2 = i/2;
    int is = i2 + j*N1d2;
    if( (i+j)%2 == 0 ) {
	    xup[is] = ((i+1)/2)%N1d2 + j*N1d2 + VOLUMEd2;
	    yup[is] = i2 + ((j+1)%N2)*N1d2 + VOLUMEd2;
	    xdn[is] = ((i-1+N1)/2)%N1d2 + j*N1d2 + VOLUMEd2;
	    ydn[is] = i2 + ((j-1+N2)%N2)*N1d2 + VOLUMEd2;
    }
    else {
	    xup[is+VOLUMEd2] = ((i+1)/2)%N1d2 + j*N1d2;
	    yup[is+VOLUMEd2] = i2 + ((j+1)%N2)*N1d2;
	    xdn[is+VOLUMEd2] = ((i-1+N1)/2)%N1d2 + j*N1d2;
	    ydn[is+VOLUMEd2] = i2 + ((j-1+N2)%N2)*N1d2;
    }
  }

  /* Initialize the measurements */
  esumt = 0.0;
  magt = 0.0;

  /* Run a number of iterations */
  for(n = 0;n < iter;n++) {
    esum = 0.0;
    mag = 0.0;

    /* Loop over the lattice and try to flip each atom */
    for(i = 0;i < VOLUME;i++) {
      float new_energy, energy_now, deltae;
      int stmp;
      int neighbours = s[xup[i]] + s[yup[i]] + s[xdn[i]] + s[ydn[i]];
      stmp = -s[i];

      /* Find the energy before and after the flip */
      energy_now = s[i]*neighbours;
      new_energy = stmp*neighbours;
      deltae = energy_now-new_energy;
      
      /* Accept or reject the change */
      if( exp(-beta*deltae) > erand48(seed) ){
	      s[i] = stmp;
	      energy_now = new_energy;
      }

      /* Measure magnetisation and energy */
      mag = mag + s[i];
      esum = esum + energy_now;
    }

    /* Calculate measurements and add to run averages  */
    esum = esum/(VOLUME);
    mag = mag/(VOLUME);
    esumt = esumt + esum;
    magt = magt + fabs(mag);

    printf("average energy = %f, average magnetization = %f\n",esum,mag);
  }

  esumt = esumt/iter;
  magt = magt/iter;
  printf("over whole iterations \n average energy = %f, average magnetization = %f\n", esumt, magt);

  return 0;
}
