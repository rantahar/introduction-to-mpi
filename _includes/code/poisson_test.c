>>int main(int argc, char** argv) {
>>
>>   int i, j;
>>   float *u, *unew, *rho;
>>   float h, hsq;
>>   double unorm;
>>
>>   /* Allocate the field u and a temporary variable unew.
>>    * The number of points in the real volume is GRIDSIZE.
>>    * Reserve space also for boundary conditions. */
>>   u    = malloc( (GRIDSIZE+2)*sizeof(float*) );
>>   unew = malloc( (GRIDSIZE+2)*sizeof(float*) );
>>   rho  = malloc( (GRIDSIZE+2)*sizeof(float*) );
>>
>>   /* Set up parameters */
>>   h = 0.1;
>> 
>>   /* Run Setup */
>>   hsq = h*h;
>> 
>>   // Initialise the u and rho field to 0 
>>   for( int i=0; i <= GRIDSIZE+1; i++ ) {
>>      u[i] = 0.0;
>>      rho[i] = 0.0;
>>   }
>>
>>   // Test a configuration with u=10 at x=0 boundary
>>   u[0] = 10;
>> 
>>   // Run a single iteration first
>>   unorm = poisson_step( u, unew, rho, hsq, GRIDSIZE );
>> 
>>   if( unorm == 25 ){
>>     printf("TEST SUCCEEDED after 1 iteration\n");
>>   } else {
>>     printf("TEST FAILED after 1 iteration\n");
>>     printf("Norm %g\n", unorm);
>>   }
>> 
>>   for( i=1; i<10; i++)
>>     unorm = poisson_step( u, unew, rho, hsq, GRIDSIZE );
>> 
>>   if( fabs(unorm - 0.463676) < 1e-6 ){
>>     printf("TEST SUCCEEDED after 10 iteration\n");
>>   } else {
>>     printf("TEST FAILED after 10 iteration\n");
>>     printf("Norm %g\n", unorm);
>>   }
>>
>>   /* Free the allocated fields */
>>   free(u);
>>   free(unew);
>>   free(rho);
>>
>>}