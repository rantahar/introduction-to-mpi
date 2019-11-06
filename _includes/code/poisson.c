/* A serial code for Poisson equation 
 * This will apply the diffusion equation to an initial state
 * untill an equilibrium state is reached. */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define GRIDSIZE 10


/* Apply a single time step */
double poisson_step( 
     float *u, float *unew, float *rho,
     float hsq, int points
   ){
   double unorm;
 
   // Calculate one timestep
   for( int i=1; i <= points; i++){
      float difference = u[i-1] + u[i+1];
      unew[i] = 0.5*( difference - hsq*rho[i] );
   }
 
   // Find the difference compared to the previous time step
   unorm = 0.0;
   for( int i = 1;i <= points; i++){
      float diff = unew[i]-u[i];
      unorm +=diff*diff;
   }
 
   // Overwrite u with the new field
   for( int i = 1;i <= points;i++){
      u[i] = unew[i];
   }
 
   return unorm;
}



int main(int argc, char** argv) {

   int i, j;
   float *u, *unew, *rho;
   float h, hsq;
   double unorm, residual;

   /* Allocate the field u and a temporary variable unew,
    * and the source field rho.
    * The number of points in the real volume is GRIDSIZE.
    * Reserve space also for boundary conditions. */
   u    = malloc( (GRIDSIZE+2)*sizeof(float*) );
   unew = malloc( (GRIDSIZE+2)*sizeof(float*) );
   rho  = malloc( (GRIDSIZE+2)*sizeof(float*) );
 
   /* Set up parameters */
   h = 0.1;
   hsq = h*h;
   residual = 1e-5;
 
   // Initialise the u and rho field to 0 
   for(i=0; i <= GRIDSIZE+1; i++) {
      u[i] = 0.0;
      rho[i] = 0.0;
   }
   
   // Create a start configuration with the heat energy
   // u=10 at the x=0 boundary
   u[0] = 10.0;
 
   // Run iterations until the field reaches an equilibrium
   for( i=0; i<10000; i++ ) {
     unorm = poisson_step( u, unew, rho, hsq, GRIDSIZE );
     printf("Iteration %d: Residue %g\n", i, unorm);
     
     if( sqrt(unorm) < sqrt(residual) ){
       break;
     }
   }
 
   printf("Run completed with residue %g\n", unorm);

   /* Free the allocated fields */
   free(u);
   free(unew);
   free(rho);
}