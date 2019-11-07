>>int main(int argc, char** argv) {
>>
>>   int i, j;
>>   float u[GRIDSIZE+2], unew[GRIDSIZE+2], rho[GRIDSIZE+2];
>>   float h, hsq;
>>   double unorm;
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
>>   // Run 9 more iterations for a total of 10
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
>>}