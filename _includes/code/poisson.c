/* a serial code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>

#define GRIDSIZE 10


double poisson_step( 
     float u[GRIDSIZE+2][GRIDSIZE+2],
     float unew[GRIDSIZE+2][GRIDSIZE+2],
     float rho[GRIDSIZE+2][GRIDSIZE+2],
     float hsq
   ){
   double unorm;
 
   // Calculate one timestep
   for( int j=1; j <= GRIDSIZE; j++){
      for( int i=1; i <= GRIDSIZE; i++){
         float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
         unew[j][i] = 0.25*( difference - hsq*rho[j][i] );
      }
   }
 
   // Find the difference compared to the previous time step
   unorm = 0.0;
   for( int j = 1;j <= GRIDSIZE; j++){
      for( int i = 1;i <= GRIDSIZE; i++){
         float diff = unew[j][i]-u[j][i];
         unorm +=diff*diff;
      }
   }
 
   // Overwrite u with the new field
   for( int j = 1;j <= GRIDSIZE;j++){
      for( int i = 1;i <= GRIDSIZE;i++){
         u[j][i] = unew[j][i];
      }
   }
 
   return unorm;
}



int main(int argc, char** argv) {

   int i, j;
   float u[GRIDSIZE+2][GRIDSIZE+2], unew[GRIDSIZE+2][GRIDSIZE+2], rho[GRIDSIZE+2][GRIDSIZE+2];
   float h, hsq;
   double unorm, residual;
 
   /* Set up parameters */
   h = 0.1;
   hsq = h*h;
   residual = 1e-5;
 
   // Initialise the u and rho field to 0 
   for(j=0; j <= GRIDSIZE+1; j++){
      for(i=0; i <= GRIDSIZE+1; i++) {
         u[j][i] = 0.0;
         rho[j][i] = 0.0;
      }
   }
   
   // Create a start configuration with the field
   // u=10 at x=0
   for(j = 0;j <= GRIDSIZE+1; j++){}
      u[j][0] = 10.0;
   }
 
   // Run iterations until the field reaches an equilibrium
   for( i=0; i<10000; i++ ) {
     
     unorm = poisson_step( u, unew, rho, hsq );
     printf("Iteration %d: Residue %g\n", i, unorm);
     
     if( sqrt(unorm) < sqrt(residual) ){
       break;
     }
   }
 
   printf("Run completed with residue %g\n", unorm);

}