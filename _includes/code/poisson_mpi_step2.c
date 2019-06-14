>> double poisson_step( 
>>   float u[GRIDSIZE+2][GRIDSIZE+2],
>>   float unew[GRIDSIZE+2][GRIDSIZE+2],
>>   float rho[GRIDSIZE+2][GRIDSIZE+2],
>>   float hsq
>> ){
>>   double unorm, global_unorm;
>>   float sendbuf[GRIDSIZE],recvbuf[GRIDSIZE];
>>   MPI_Status mpi_status;
>>   int rank, n_ranks, my_j_max;
>>  
>>   /* Find the number of x-slices calculated by each rank */
>>   /* The simple calculation here assumes that GRIDSIZE is divisible by n_ranks */
>>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>   my_j_max = GRIDSIZE/n_ranks;
>>    
>>   // Calculate one timestep
>>   for( int j=1; j <= my_j_max; j++){
>>     for( int i=1; i <= GRIDSIZE; i++){
>>         float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>>  	    unew[j][i] =0.25*( difference - hsq*rho[j][i] );
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
>>   for( int j = 1;j <= my_j_max; j++){
>>     for( int i = 1;i <= GRIDSIZE; i++){
>>       u[j][i] = unew[j][i];
>>     }
>>   }
>>  
>>   // The u field has been changed, communicate it to neighbours
>>   // With blocking communication, half the ranks should send first
>>   // and the other half should receive first
>>   if ((rank%2) == 1) {
>>     // Ranks with odd number send first
>>  
>>     // Send data down from rank to rank-1
>>     for( int i=0;i < GRIDSIZE;i++){
>>       sendbuf[i] = unew[1][i+1];
>>     }
>>     MPI_Send(sendbuf,GRIDSIZE,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD);
>>     // Receive dat from rank-1
>>     MPI_Recv(recvbuf,GRIDSIZE,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD,&mpi_status);
>>     for( int i=0;i < GRIDSIZE;i++){
>>       u[0][i+1] = recvbuf[i];
>>     }
>>      
>>     if ( rank != (n_ranks-1)) {
>>       // Send data up to rank+1 (if I'm not the last rank)
>>       for( int i=0;i < GRIDSIZE;i++){
>>         sendbuf[i] = unew[my_j_max][i+1];
>>       }
>>       MPI_Send(sendbuf,GRIDSIZE,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD);
>>       // Receive data from rank+1
>>       MPI_Recv(recvbuf,GRIDSIZE,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD,&mpi_status);
>>       for( int i=0;i < GRIDSIZE;i++){
>>         u[my_j_max+1][i+1] = recvbuf[i];
>>       }
>>     }
>>   
>>   } else {
>>     // Ranks with even number receive first
>>  
>>     if (rank != 0) {
>>       // Receive data from rank-1 (if I'm not the first rank)
>>       MPI_Recv(recvbuf,GRIDSIZE,MPI_FLOAT,rank-1,1,MPI_COMM_WORLD,&mpi_status);
>>       for( int i=0;i < GRIDSIZE;i++){
>>         u[0][i+1] = recvbuf[i];
>>       }
>>  	     
>>       // Send data down to rank-1
>>       for( int i=0;i < GRIDSIZE;i++){
>>         sendbuf[i] = unew[1][i+1];
>>       }
>>       MPI_Send(sendbuf,GRIDSIZE,MPI_FLOAT,rank-1,2,MPI_COMM_WORLD);
>>     }
>>  
>>     if (rank != (n_ranks-1)) {
>>       // Receive data from rank+1 (if I'm not the last rank)
>>       MPI_Recv(recvbuf,GRIDSIZE,MPI_FLOAT,rank+1,1,MPI_COMM_WORLD,&mpi_status);
>>       for( int i=0;i < GRIDSIZE;i++){
>>         u[my_j_max+1][i+1] = recvbuf[i];
>>       }
>>       
>>       // Send data up to rank+1
>>       for( int i=0;i < GRIDSIZE;i++){
>>         sendbuf[i] = unew[my_j_max][i+1];
>>       }
>>       MPI_Send(sendbuf,GRIDSIZE,MPI_FLOAT,rank+1,2,MPI_COMM_WORLD);
>>     }
>>   }
>>  
>>   return global_unorm;
>> }