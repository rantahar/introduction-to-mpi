>> double poisson_step( 
>>     float u[GRIDSIZE+2][GRIDSIZE+2],
>>     float unew[GRIDSIZE+2][GRIDSIZE+2],
>>     float rho[GRIDSIZE+2][GRIDSIZE+2],
>>     float hsq
>>   ){
>>   /* We will calculate unorm separately on each rank. It needs to be summed up at the end*/
>>   double unorm;
>>   /* This will be the sum over all ranks */
>>   double global_unorm;
>>   int n_ranks, rank, my_j_max;
>> 
>>    /* Find the number of x-slices calculated by each rank */
>>    /* The simple calculation here assumes that GRIDSIZE is divisible by n_ranks */
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>    my_j_max = GRIDSIZE/n_ranks;
>> 
>>   // Calculate one timestep
>>   for( int j=1; j <= my_j_max; j++){
>>     for( int i=1; i <= GRIDSIZE; i++){
>>        float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>>        unew[j][i] =0.25*( difference - hsq*rho[j][i] );
>>     }
>>   }
>> 
>>   // Find the difference compared to the previous time step
>>   unorm = 0.0;
>>   for( int j = 1;j <= my_j_max; j++){
>>     for( int i = 1;i <= GRIDSIZE; i++){
>>       float diff = unew[j][i]-u[j][i];
>>       unorm +=diff*diff;
>>     }
>>   }
>> 
>>   // Use Allreduce to calculate the sum over ranks
>>   MPI_Allreduce( &unorm, &global_unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
>> 
>>   // Overwrite u with the new field
>>   for( int j = 1;j <= my_j_max;j++){
>>     for( int i = 1;i <= GRIDSIZE;i++){
>>       u[j][i] = unew[j][i];
>>     }
>>   }
>> 
>>   // Return the sum over all ranks
>>   return global_unorm;
>> }
>>