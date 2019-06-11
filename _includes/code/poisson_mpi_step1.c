>> 
>> #include <stdio.h>
>> #include <math.h>
>> #include <mpi.h>
>> 
>> #define MAX 10
>> 
>> 
>> double poisson_step( 
>>     float u[MAX+2][MAX+2],
>>     float unew[MAX+2][MAX+2],
>>     float rho[MAX+2][MAX+2],
>>     float hsq
>>   ){
>>   /* We will calculate unorm separately on each rank. It needs to be summed up at the end*/
>>   double unorm;
>>   /* This will be the sum over all ranks */
>>   double global_unorm;
>>   int n_ranks, rank, my_j_max;
>> 
>>    /* Find the number of x-slices calculated by each rank */
>>    /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>    my_j_max = MAX/n_ranks;
>> 
>>   // Calculate one timestep
>>   for( int j=1; j <= my_j_max; j++){
>>     for( int i=1; i <= MAX; i++){
>>        float difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i];
>>        unew[j][i] =0.25*( difference - hsq*rho[j][i] );
>>     }
>>   }
>> 
>>   // Find the difference compared to the previous time step
>>   unorm = 0.0;
>>   for( int j = 1;j <= my_j_max; j++){
>>     for( int i = 1;i <= MAX; i++){
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
>>     for( int i = 1;i <= MAX;i++){
>>       u[j][i] = unew[j][i];
>>     }
>>   }
>> 
>>   // Return the sum over all ranks
>>   return global_unorm;
>> }
>> 
>> 
>> 
>> int main(int argc, char** argv) {
>> 
>>    int i,j;
>>    float u[MAX+2][MAX+2], unew[MAX+2][MAX+2], rho[MAX+2][MAX+2];
>>    float h, hsq;
>>    double unorm;
>>    int rank, n_ranks, my_j_max;
>> 
>>    // First call MPI_Init
>>    MPI_Init(&argc, &argv);
>> 
>>    /* Find the number of x-slices calculated by each rank */
>>    /* The simple calculation here assumes that MAX is divisible by n_ranks */
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>    my_j_max = MAX/n_ranks;
>> 
>>    /* Set up parameters */
>>    h = 0.1;
>>  
>>    /* Run Setup */
>>    hsq = h*h;
>> 
>>    // Initialise the u and rho field to 0 
>>    for( int j=0; j <= my_j_max+1; j++ ){
>>       for( int i=0; i <= MAX+1; i++ ) {
>>          u[j][i] = 0.0;
>>          rho[j][i] = 0.0;
>>       }
>>    }
>> 
>>    // Start form a configuration with u=10 at x=1 and y=1
>>    // The actual x coordinate is my_j_max*rank + x
>>    // meaning that x=1 is on rank 0
>>    if( rank == 0 )
>>       u[1][1] = 10;
>>     
>>    // Run a single iteration first
>>    unorm = poisson_step( u, unew, rho, hsq );
>>  
>>    if( unorm == 112.5 ){
>>      printf("TEST SUCCEEDED after 1 iteration\n");
>>    } else {
>>      printf("TEST FAILED after 1 iteration\n");
>>      printf("Norm %g\n", unorm);
>>    }
>>  
>>    for( i=1; i<10; i++)
>>      unorm = poisson_step( u, unew, rho, hsq );
>>  
>>    if( fabs(unorm - 0.208634816) < 1e-6 ){
>>      printf("TEST SUCCEEDED after 10 iteration\n");
>>    } else {
>>      printf("TEST FAILED after 10 iteration\n");
>>      printf("Norm %g\n", unorm);
>>    }
>> 
>>    // Call finalize at the end
>>    MPI_Finalize();
>> }