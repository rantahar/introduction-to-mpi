---
title: "MPI_Send and MPI_Recv"
teaching: 10
exercises: 20
questions:
- "How do I send data from one process to another"
objectives:
- "Introduce the MPI_Send and MPI_Recv functions"
keypoints:
- "Use MPI_Send to send messages"
- "And MPI_Recv to receive them"
- "MPI_Recv will block the program until the message is received"
---

In this section we will use two MPI library functions functions for sending and receiving data.
These are MPI_Send and MPI_Recv.

These two functions together accomplish the task of sending and receiving data
from one rank to another.
They are the basic building blocks for essentially all of the more
specialized MPI commands described later.
They are also the basic communication tools in your MPI application.

The process of communicating data follows a standard pattern.
Rank A decides to send data to rank B.
If first packs the data into a buffer.
This avoids sending multiple messages, which would take more time.
Rank A then calls MPI_Send to create a message for rank B.
The communication device is then given the responsibility of routing
the message to the correct destination.

Rank B must know that it is about to receive a message and acknowledge this
by calling MPI_Recv.
This instructs the communication device to listen for incoming data.

> ## MPI_Send and MPI_Recv in C
>
>~~~
> MPI_Send(
>    void* data,
>    int count,
>    MPI_Datatype datatype,
>    int destination,
>    int tag,
>    MPI_Comm communicator)
>~~~
>
> | data:         | Pointer to the start of the data being sent |
> | count:        | Number of elements to send |
> | datatype:     | The type of the data being sent |
> | destination:  | The rank number of the rank the data will be sent to |
> | tag:          | A message tag (integer) |
> | communicator: | The communicator (we have used MPI_COMM_WORLD in earlier) |
>
>~~~
> MPI_Recv(
>    void* data,
>    int count,
>    MPI_Datatype datatype,
>    int source,
>    int tag,
>    MPI_Comm communicator,
>    MPI_Status* status)
>~~~
>
> | data:         | Pointer to where the received data should be written |
> | count:        | Maximum number of elements received |
> | datatype:     | The type of the data being received |
> | source:       | The rank number of the rank sending the data |
> | tag:          | A message tag (integer) |
> | communicator: | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | status:       | A pointer for writing the exit status of the MPI command |
>
{: .prereq .foldable}

> ## MPI_Send and MPI_Recv in Fortran
>
>~~~
> MPI_SEND(BUF, COUNT, DATATYPE, DEST, TAG, COMM, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, DEST, TAG, COMM, IERROR
>~~~
>
> | BUF:      | Vector containing the data to send |
> | COUNT:    | Number of elements to send |
> | DATATYPE: | The type of the data being sent |
> | DEST:     | The rank number of the rank the data will be sent to |
> | TAG:      | A message tag (integer) |
> | COMM:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | IERROR:   | Error status |
>
>~~~
> MPI_RECV(BUF, COUNT, DATATYPE, SOURCE, TAG, COMM, STATUS, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, SOURCE, TAG, COMM
>    INTEGER    STATUS(MPI_STATUS_SIZE), IERROR
>~~~
>
> | BUF:      | Vector the received data should be written to             |
> | COUNT:    | Maximum number of elements received                       |
> | DATATYPE: | The type of the data being received                       |
> | SOURCE:   | The rank number of the rank sending the data              |
> | TAG:      | A message tag (integer)                                   |
> | COMM:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | STATUS:   | A pointer for writing the exit status of the MPI command  |
>
{: .prereq .foldable}

The number of arguments can make these commands look complicated,
but you will quickly get used to it.
The first four arguments are straightforward, they
specify what data needs to be sent or received
and the destination or source of the message.

The message tag is used to differentiate messages in case rank A has sent
multiple pieces of data to rank B.
When rank B requests for a message with the correct tag, the data buffer will
be overwritten by that message.

The communicator is something we have seen before.
It specifies information about the system and where each rank actually is.
The status parameter in MPI_Recv will give information about any possible problems
in transit.

> ## Example in C
> ~~~
> #include <stdio.h>
> #include <math.h>
> #include <mpi.h>
>
> main(int argc, char** argv) {
>   int rank, n_ranks, numbers_per_rank;
>   int my_first, my_last;
>   int numbers = 10;
>
>   // Firt call MPI_Init
>   MPI_Init(&argc, &argv);
>
>   // Check that there are at least two ranks
>   MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
>   if( n_ranks < 2 ){
>     printf("This example requires at least two ranks");
>     MPI_Finalize();
>     return(1);
>   }
>   
>   // Get my rank
>   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
>
>   if( rank == 0 ){
>      char *message = "Hello, world!";
>      // Note that MPI_BYTE is the MPI type for char
>      MPI_Send(message, 13, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
>   }
>
>   if( rank == 1 ){
>      char message[13];
>      int status;
>      // Note that MPI_BYTE is the MPI type for char
>      MPI_Recv(message, 13, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
>      printf("%s",message)
>   }
>   
>   // Call finalize at the end
>   MPI_Finalize();
> }
> ~~~
>
{: .prereq .foldable}

> ## Try It Out
>
> Compile and run the above code
>
{: .challenge}



> ## Sending and Receiving
>
> Modify the Hello World code so that each rank sends its
> message to rank 0. Have rank 0 print each message.
>
> > ## Hello World in C
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > main(int argc, char** argv) {
> >   int rank, n_ranks, numbers_per_rank;
> >
> >   // Firt call MPI_Init
> >   MPI_Init(&argc, &argv);
> >   // Get my rank and the number of ranks
> >   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >   MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> >
> >   printf("Hello! World = %d\n", rank);
> >   printf("total no. of nodes = %d\n", n_ranks);
> >
> >   // Call finalize at the end
> >   MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .prereq .foldable}
>
> > ## Hello World in Fortran
> > ~~~
> >
> >      program hello
> >
> >      include 'mpif.h'
> >      
> >      integer rank, n_ranks
> >
> >      call MPI_INIT(ierr)
> >      call MPI_COMM_RANK(MPI_COMM_WORLD,rank,ierr)
> >      call MPI_COMM_SIZE(MPI_COMM_WORLD,n_ranks,ierr)
> >      write(6,*) 'Hello! World = ', rank
> >      write(6,*) 'total no. of nodes = ", n_ranks
> >      call MPI_FINALIZE(ierr)
> >
> >      stop
> >      end
> >
> > ~~~
> > {: .output}
> {: .prereq .foldable}
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
> {: .solution}
>
{: .challenge}



> ## Blocking
>
> - Try this code and see what happens
> - How would you change the code to fix the problem?
>
> > ## C
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > int main(int argc, char** argv) {
> >    int rank, n_ranks, neighbour;
> >    int n_numbers = 1048576;
> >    int send_message[n_numbers];
> >    int recv_message[n_numbers];
> >    MPI_Status status;
> > 
> >    // Firt call MPI_Init
> >    MPI_Init(&argc, &argv);
> > 
> >    // Get my rank and the number of ranks
> >    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> > 
> >    // Call the other rank the neighbour
> >    if( rank == 0 ){
> >       neighbour = 1;      
> >    } else {
> >       neighbour = 0;
> >    }
> > 
> >    // Generate numbers to send
> >    for( int i=0; i<n_numbers; i++){
> >       send_message[i] = i;
> >    }
> > 
> >    // Send the message to other rank
> >    MPI_Send(send_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD);
> > 
> >    // Receive the message from the other rank
> >    MPI_Recv(recv_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &status);
> >    printf("Message received by rank %d \n", rank);
> > 
> >    // Call finalize at the end
> >    MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .prereq .foldable}
>
>
>
> > ## Solution in C
> > 
> > MPI_Send will block execution until until the receiving process has called
> > MPI_Recv. This prevents the sender from unintentionally modifying the message
> > buffer before the message is actually sent.
> > Above, both ranks call MPI_Send and just wait for the other respond.
> > The solution is to have one of the ranks receive it's message before sending:
> >
> > Sometimes MPI_Send will actually make a copy of the buffer and return immediately.
> > This generally happens only for short messages.
> > Even when this happens, the actual transfer will not start before the receive is posted.
> > 
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > int main(int argc, char** argv) {
> >    int rank, n_ranks, neighbour;
> >    int n_numbers = 1048576;
> >    int send_message[n_numbers];
> >    int recv_message[n_numbers];
> >    MPI_Status status;
> > 
> >    // Firt call MPI_Init
> >    MPI_Init(&argc, &argv);
> > 
> >    // Get my rank and the number of ranks
> >    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> > 
> >    // Generate numbers to send
> >    for( int i=0; i<n_numbers; i++){
> >       send_message[i] = i;
> >    }
> > 
> >    if( rank == 0 ){
> >       // Rank 0 will send first
> >       MPI_Send(send_message, n_numbers, MPI_INT, 1, 0, MPI_COMM_WORLD);
> >    }
> > 
> >    if( rank == 1 ){
> >       // Rank 1 will receive it's message before sending
> >       MPI_Recv(recv_message, n_numbers, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
> >       printf("Message received by rank %d \n", rank);
> >    }
> > 
> >    if( rank == 1 ){
> >       // Now rank 1 is free to send
> >       MPI_Send(send_message, n_numbers, MPI_INT, 0, 0, MPI_COMM_WORLD);
> >    }
> > 
> >    if( rank == 0 ){
> >       // And rank 0 will receive the message
> >       MPI_Recv(recv_message, n_numbers, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
> >       printf("Message received by rank %d \n", rank);
> >    }
> > 
> >    // Call finalize at the end
> >    MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .solution}
>
>
{: .challenge}


> ## Ping Pong
>
> Write a simplified simplified simulation of pingpong according to the
> following rules:
> * Ranks 0 and 1 participate
> * Rank 0 starts with the ball
> * The rank with the ball sends it to the other rank
> * Both ranks count the number of times they get the ball
> * After counting to 1 million, the rank gives up
> * There are no misses or points
>
>
> > ## Solution in C
> > 
> > ~~~
> > #include <stdio.h>
> > #include <mpi.h>
> > 
> > int main(int argc, char** argv) {
> >    int rank, n_ranks, neighbour;
> >    int max_count = 1000000;
> >    int counter;
> >    int bored;
> >    int ball = 1; // A dummy message to simulate the ball
> >    MPI_Status status;
> > 
> >    // Firt call MPI_Init
> >    MPI_Init(&argc, &argv);
> > 
> >    // Get my rank and the number of ranks
> >    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
> >    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
> > 
> >    // Call the other rank the neighbour
> >    if( rank == 0 ){
> >        neighbour = 1;
> >    } else {
> >        neighbour = 0;
> >    }
> > 
> >    if( rank == 0 ){
> >       // Rank 0 starts with the ball. Send it to rank 1
> >       MPI_Send(&ball, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
> >    }
> > 
> >    counter = 0;
> >    bored = 0;
> >    while( !bored )
> >    {
> >       // Receive the ball
> >       MPI_Recv(&ball, 1, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &status);
> >       
> >       // Increment the counter
> >       counter++;
> > 
> >       // Send the ball back
> >       MPI_Send(&ball, 1, MPI_INT, neighbour, 0, MPI_COMM_WORLD);
> > 
> > 
> >       // Check if the rank is bored
> >       bored = counter < max_count;
> >    }
> >    printf("rank %d is bored and giving up \n", rank);
> > 
> >    // Call finalize at the end
> >    MPI_Finalize();
> > }
> > ~~~
> > {: .output}
> {: .solution}
>
>
{: .challenge}



{% include links.md %}

