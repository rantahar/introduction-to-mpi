/* a serial code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>

#define MAX 100
#define IMAX 1000


void main(int argc, char** argv) {

  int i, j;
  float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
  float h, hsq;
  double unorm, residual;

  FILE *fopen(), *fp12, *fp14, *fp10;

  // Read parameters from the command line
  printf("step size = \n"); 
  scanf("%f",&h);
  printf("step size = %f\n",h);

  printf("residual = \n");
  scanf("%lf",&residual);
  printf("residual = %.7f\n",residual);
  
  // Open output files
  fp12 = fopen("c.12","w");
  fp14 = fopen("c.14","w");
  fp10 = fopen("c.10","w");


  /* Run Setup */
  hsq = h*h;

  // Initialise the u and rho field to 0 
  for(j=0; j <= MAX+1; j++){
    for(i=0; i <= MAX+1; i++) {
      u[i][j] = 0.0;
      rho[i][j] = 0.0;
    }
  }
  
  // Create a start configuration with the field
  // u=10 at x=0
  for(j = 0;j <= MAX+1; j++)
    u[0][j] = 10.0;


  // Run iterations until the field reaches an equilibrium
  do {

    // Calculate one timestep
    for(j = 1;j <= MAX; j++){
      for(i = 1;i <= MAX; i++){
          double difference = u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1];
	        unew[i][j] =0.25*( difference - hsq*rho[i][j] );
      }
    }

    // Find the difference compared to the previous time step
    unorm = 0.0;
    for(j = 1;j <= MAX; j++){
      for(i = 1;i <= MAX; i++){
        double diff = unew[i][j]-u[i][j];
        unorm +=diff*diff;
      }
    }
    printf("unorm = %.8e\n",unorm);

    // Overwrite u with the new field
    for(j = 1;j <= MAX;j++){
      for(i = 1;i <= MAX;i++){
        u[i][j] = unew[i][j];
      }
    }
    
  } while( sqrt(unorm) > sqrt(residual) );

  /* The run is complete. Write the fields into the result files.*/
  // Write the field in the middle in the y direction
  for(i = 0;i <= MAX+1; i++)
    fprintf(fp12,"%f\n", u[i][MAX/2+1]);

  // Write the field at x=10
  for(i = 0;i <= MAX+1; i++)
    fprintf(fp14,"%f\n", u[10+1][i]);

  // Write the whole u field
  for(j = 0;j <= MAX+1; j++){
    for(i = 0;i <= MAX+1; i++){
      fprintf(fp10,"%f\n",u[i][j]);
    }
  }
}
