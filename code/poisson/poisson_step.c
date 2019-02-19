/* a serial code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>

#define MAX 100
#define IMAX 1000


double poisson_step( 
    float u[MAX+2][MAX+2],
    float unew[MAX+2][MAX+2],
    float rho[MAX+2][MAX+2],
    float hsq
  ){
  double unorm;

  // Calculate one timestep
  for( int j=1; j <= MAX; j++){
    for( int i=1; i <= MAX; i++){
        float difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
	    unew[i][j] =0.25*( difference - hsq*rho[i][j] );
    }
  }

  // Find the difference compared to the previous time step
  unorm = 0.0;
  for( int j = 1;j <= MAX; j++){
    for( int i = 1;i <= MAX; i++){
      float diff = unew[i][j]-u[i][j];
      unorm +=diff*diff;
    }
  }
  printf("unorm = %.8e\n",unorm);

  // Overwrite u with the new field
  for( int j = 1;j <= MAX;j++){
    for( int i = 1;i <= MAX;i++){
      u[i][j] = unew[i][j];
    }
  }

  return unorm;
}
