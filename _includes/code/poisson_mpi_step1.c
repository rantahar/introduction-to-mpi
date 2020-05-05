>> double poisson_step( 
>>     float *u, float *unew, float *rho,
>>     float hsq, int points
>>   ){
>>   /* We will calculate unorm separately on each rank. It needs to be summed up at the end*/
>>   double unorm;
>>   /* This will be the sum over all ranks */
>>   double global_unorm;
>> 
>>   // Calculate one timestep
>>   for( int i=1; i <= points; i++){
>>      float difference = u[i-1] + u[i+1];
>>      unew[i] = 0.5*( difference - hsq*rho[i] );
>>   }
>> 
>>   // Find the difference compared to the previous time step
>>   unorm = 0.0;
>>   for( int i = 1;i <= points; i++){
>>      float diff = unew[i]-u[i];
>>      unorm +=diff*diff;
>>   }
>> 
>>   // Use Allreduce to calculate the sum over ranks
>>   MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
>> 
>>   // Overwrite u with the new field
>>   for( int i = 1;i <= points;i++){
>>      u[i] = unew[i];
>>   }
>> 
>>   // Return the sum over all ranks
>>   return global_unorm;
>> }
>>