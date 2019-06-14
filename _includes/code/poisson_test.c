>>int main(int argc, char** argv) {
>>
>>   int i, j;
>>   float u[GRIDSIZE+2][GRIDSIZE+2], unew[GRIDSIZE+2][GRIDSIZE+2], rho[GRIDSIZE+2][GRIDSIZE+2];
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
>>   for( int j=0; j <= GRIDSIZE+1; j++ ){
>>      for( int i=0; i <= GRIDSIZE+1; i++ ) {
>>         u[j][i] = 0.0;
>>         rho[j][i] = 0.0;
>>      }
>>   }
>>
>>   // Test a configuration with u=10 at x=1 and y=1
>>   u[1][1] = 10;
>> 
>>   // Run a single iteration first
>>   unorm = poisson_step( u, unew, rho, hsq );
>> 
>>   if( unorm == 112.5 ){
>>     printf("TEST SUCCEEDED after 1 iteration\n");
>>   } else {
>>     printf("TEST FAILED after 1 iteration\n");
>>   }
>> 
>>   for( i=1; i<10; i++)
>>     unorm = poisson_step( u, unew, rho, hsq );
>> 
>>   if( fabs(unorm - 0.208634816) < 1e-6 ){
>>     printf("TEST SUCCEEDED after 10 iteration\n");
>>   } else {
>>     printf("TEST FAILED after 10 iteration\n");
>>   }
>>
>>}