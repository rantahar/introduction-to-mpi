>> double poisson_step( 
>>   float *u, float *unew, float *rho,
>>   float hsq, int points
>> ){
>>   double unorm, global_unorm;
>>   float sendbuf, recvbuf;
>>   MPI_Status mpi_status;
>>   int rank, n_ranks;
>>  
>>   /* Find the number of x-slices calculated by each rank */
>>   /* The simple calculation here assumes that GRIDSIZE is divisible by n_ranks */
>>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
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
>>   // The u field has been changed, communicate it to neighbours
>>   // With blocking communication, half the ranks should send first
>>   // and the other half should receive first
>>   if ((rank%2) == 1) {
>>     // Ranks with odd number send first
>>  
>>     // Send data down from rank to rank-1
>>     sendbuf = unew[1];
>>     MPI_Send(&sendbuf,1,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD);
>>     // Receive dat from rank-1
>>     MPI_Recv(&recvbuf,1,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD,&mpi_status);
>>     u[0] = recvbuf;
>>      
>>     if ( rank != (n_ranks-1)) {
>>       // Send data up to rank+1 (if I'm not the last rank)
>>       MPI_Send(&u[points],1,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD);
>>       // Receive data from rank+1
>>       MPI_Recv(&u[points+1],1,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD,&mpi_status);
>>     }
>>   
>>   } else {
>>     // Ranks with even number receive first
>>  
>>     if (rank != 0) {
>>       // Receive data from rank-1 (if I'm not the first rank)
>>       MPI_Recv(&u[0],1,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD,&mpi_status);
>>       // Send data down to rank-1
>>       MPI_Send(&u[1],1,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD);
>>     }
>>  
>>     if (rank != (n_ranks-1)) {
>>       // Receive data from rank+1 (if I'm not the last rank)
>>       MPI_Recv(&u[points+1],1,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD,&mpi_status);
>>       // Send data up to rank+1
>>       MPI_Send(&u[points],1,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD);
>>     }
>>   }
>>  
>>   return global_unorm;
>> }