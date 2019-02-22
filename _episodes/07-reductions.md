---
title: "Collective Operations"
teaching:  10
exercises: 20
questions:
- "What other useful functions does MPI have?"
objectives:
- "Introduce collective operations"
keypoints:
- "Use MPI_Barrier for global synchronisation"
- "All-to-All, One-to-All and All-to_one communications have efficient implementation in the library."
- "There are functions for global reductions. Don't write your own."
---

## Collective Operations

There are several special cases that deserve their own implementations in the MPI standard.
The most common are:

* Synchronisation
  * Wait until all processes have reached the same point in the program.
  * MPI_Barrier()
  * Mainly useful for debugging and solving timing related problems.
* One-To-All Communication: 
  * One rank send the same message to all other ranks
  * MPI_Bcast(), MPI_Scatter()
  * Useful for sending input or commands to other all ranks
* All-to-One
  * All ranks send data to a single rank
  * MPI_Reduce(), MPI_Gather()
* All-to-All
  * All ranks have data and all ranks receive data
  * Global reductions is an important special case
  * MPI_Alltoall(), MPI_Allgather
  * MPI_Allreduce()

### Barrier

> ## Barrier in C
>
>~~~
> MPI_Barrier( MPI_Comm communicator )
>~~~
>
{: .prereq .foldable}

Just wait until all ranks have reached this line.

### Broadcast

> ## Broadcast in C
> ~~~
>  MPI_Bcast(
>     void* data,
>     int count,
>     MPI_Datatype datatype,
>     int root,
>     MPI_Comm communicator)
> ~~~
>
{: .prereq .foldable}

Very similar to MPI_Send, but the same data is sent from root to all ranks.
This function will only return once all processes have reached it, 
making it a kind of a barrier.

### Scatter

> ## Scatter in C
> ~~~
>  MPI_Scatter(
>     void* send-buffer,
>     int send-count,
>     MPI_Datatype send-datatype,
>     void* receive-buffer,
>     int receive-count,
>     MPI_Datatype receive-datatype,
>     int root,
>     MPI_Comm communicator)
> ~~~
>
{: .prereq .foldable}

The data in the roots send-buffer is split into chunks descrived by the receive-count
and each chunk is sent to a different rank.
The received data is written to the receive-buffer, so the send-buffer is only
needed by the root.

### Gather

> ## Gather in C
> ~~~
>  MPI_Gather(
>     void* send-buffer,
>     int send-count,
>     MPI_Datatype send-datatype,
>     void* receive-buffer,
>     int receive-count,
>     MPI_Datatype receive-datatype,
>     int root,
>     MPI_Comm communicator)
> ~~~
>
{: .prereq .foldable}

Each rank sends the data in the send-buffer to the root.
The root collect the data into the receive-buffer in order of the rank
numbers.


> ## Sending and Receiving
>
> In the morning we wrote a hello world program where each rank
> sends a message to rank 0.
> Write this using a gather instead of send and receive.
>
> > ## Solution in C
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > int main(int argc, char** argv) {
> >   int rank, n_ranks, numbers_per_rank;
> >
> >   // Firt call MPI_Init
> >   MPI_Init(&argc, &argv);
> >   // Get my rank and the number of ranks
> >   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> >
> >   if( rank != 0 ){
> >      // All ranks other than 0 should send a message
> >
> >      char message[20];
> >      sprintf(message, "Hello World, I'm rank %d\n", rank);
> >      MPI_Send(message, 20, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
> >
> >   } else {
> >      // Rank 0 will receive each message and print them
> >
> >      for( int r=1; r<n_ranks; r++ ){
> >         char message[20];
> >         MPI_Status status;
> >
> >         MPI_Recv(message, 13, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
> >         printf("%s",message);
> >      }
> >   }
> >   
> >   // Call finalize at the end
> >   MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .prereq .foldable}
>
>
>
{: .challenge}


### Reduce

> ## Reduce in C
> ~~~
>  MPI_Reduce(
>     void* send-buffer,
>     void* receive-buffer,
>     int count,
>     MPI_Datatype datatype,
>     MPI_Op operation,
>     int root,
>     MPI_Comm communicator)
> ~~~
>
{: .prereq .foldable}

Each rank sends a piece data, which are combined on their to the roon into a single piece of data.
For example, the function can calculate the sum of numbers distributed accross
all the ranks.

Possible operations include
* MPI_SUM: Calculate the sum of numbers sent by each rank
* MPI_MAX: Return the maximum value of numbers sent by each rank
* MPI_MIN: Return the minimum of numbers sent by each rank
* MPI_PROD: Calculate the product of numbers sent by each rank
* MPI_MAX: Return the maximum value and the number of the rank that sent the maximum value
* MPI_MIN: Return the minimum value of the number of the rank that sent the minimum value

The MPI_Reduce operation is usually faster than what you would write by hand.
It can different algorithms depending on the system it's running on to reach best
possible performance.
Especially on systems designed for high performance computing, the MPI_Reduce operations
can use the communication devices to perform reductions en route, without using any
of the ranks to do the calculation.


### Allreduce

> ## Allreduce in C
> ~~~
>  MPI_Allreduce(
>     void* send-buffer,
>     void* receive-buffer,
>     int count,
>     MPI_Datatype datatype,
>     MPI_Op operation,
>     MPI_Comm communicator)
> ~~~
>
{: .prereq .foldable}

MPI_Allreduce is performs essentially the same operations as MPI_Reduce,
but the result is sent to all the ranks.


> ## Reductions
>
> Replace all three for-loops in the example code using MPI_Reduce or
> MPI_Allreduce.
>
> > ## C
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > // Calculate the sum of numbers in a vector
> > double find_sum( double * vector, int N ){
> >    double sum = 0;
> >    for( int i=0; i<N; i++){
> >       sum += vector[i];
> >    }
> >    return sum;
> > }
> > 
> > // Find the maximum of numbers in a vector
> > double find_maximum( double * vector, int N ){
> >    double max = 0;
> >    for( int i=0; i<N; i++){
> >       if( vector[i] > max ){
> >          max = vector[i];
> >       }
> >    }
> >    return max;
> > }
> > 
> > 
> > int main(int argc, char** argv) {
> >    int n_numbers = 1024;
> >    int rank;
> >    double vector[n_numbers];
> >    double sum, max;
> >    double my_first_number;
> > 
> >    // Firt call MPI_Init
> >    MPI_Init(&argc, &argv);
> > 
> >    // Get my rank and the number of ranks
> >    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> > 
> >    // Each rank will have n_numbers numbers,
> >    // starting from where the previous left
> >    my_first_number = n_numbers*rank;
> > 
> >    // Generate a vector
> >    for( int i=0; i<n_numbers; i++){
> >       vector[i] = my_first_number + i;
> >    }
> > 
> >    //Find the sum and print
> >    sum = find_sum( vector, n_numbers );
> >    printf("The sum of the numbers is %f\n", sum);
> > 
> >    //Find the maximum and print
> >    max = find_maximum( vector, n_numbers );
> >    printf("The largest number is %f\n", max);
> > 
> >    // Call finalize at the end
> >    MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .prereq .foldable}
>
>
> > ## Solution in C
> > 
> > ~~~
> > // Calculate the sum of numbers in a vector
> > double find_sum( double * vector, int N ){
> >    double sum = 0;
> >    double global_sum;
> > 
> >    // Calculate the sum on this rank as befor
> >    for( int i=0; i<N; i++){
> >       sum += vector[i];
> >    }
> > 
> >    // Call MPI_Allreduce to find the full sum
> >    MPI_Allreduce( &sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
> > 
> >    return global_sum;
> > }
> > 
> > // Find the maximum of numbers in a vector
> > double find_maximum( double * vector, int N ){
> >    double max = 0;
> >    double global_max;
> > 
> >    // Calculate the sum on this rank as before
> >    for( int i=0; i<N; i++){
> >       if( vector[i] > max ){
> >          max = vector[i];
> >       }
> >    }
> > 
> >    // Call MPI_Allreduce to find the maximum over all the ranks
> >    MPI_Allreduce( &max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD );
> > 
> >    return global_max;
> > }
> > ~~~
> > {: .output}
> {: .solution}
>
{: .challenge}



{% include links.md %}

