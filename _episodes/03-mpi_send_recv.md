---
title: "MPI_Send and MPI_Recv"
teaching: 5
exercises: 20
questions:
- "How do I send data from one rank to another"
objectives:
- "Introduce the `MPI_Send` and `MPI_Recv` functions"
keypoints:
- "Use `MPI_Send` to send messages"
- "And `MPI_Recv` to receive them"
- "`MPI_Recv` will block the program until the message is received"
---

<!-- Actual timing, challenges 17, all 25. Skipped several challenges -->

## Communication
In this section we will use two MPI library functions functions to send data from one rank to another.
These are `MPI_Send` and `MPI_Recv`.
They are the basic building blocks for essentially all of the more
specialized MPI commands described later.
They are also the basic communication tools in your MPI application.
Since `MPI_Send` and `MPI_Recv` involve two ranks, they are called "point-to-point" communication
(unlike "global" communication mentioned in lesson 2).

The process of communicating data follows a standard pattern.
Rank A decides to send data to rank B.
It first packs the data into a buffer.
This avoids sending multiple messages, which would take more time.
Rank A then calls `MPI_Send` to create a message for rank B.
The communication device is then given the responsibility of routing
the message to the correct destination.

Rank B must know that it is about to receive a message and acknowledge this
by calling `MPI_Recv`.
This sets up a buffer for writing the incoming data
and instructs the communication device to listen for the message.
The message will not actually be sent before the receiving rank call MPI_Recv,
even if MPI_Send has been called.

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
> | `data`:         | Pointer to the start of the data being sent |
> | `count`:        | Number of elements to send |
> | `datatype`:     | The type of the data being sent |
> | `destination`:  | The rank number of the rank the data will be sent to |
> | `tag`:          | A message tag (integer) |
> | `communicator`: | The communicator (we have used `MPI_COMM_WORLD` in earlier) |
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
> | `data`:         | Pointer to where the received data should be written |
> | `count`:        | Maximum number of elements received |
> | `datatype`:     | The type of the data being received |
> | `source`:       | The rank number of the rank sending the data |
> | `tag`:          | A message tag (integer) |
> | `communicator`: | The communicator (we have used `MPI_COMM_WORLD` in earlier) |
> | `status`:       | A pointer for writing the exit status of the MPI command |
>
{: .prereq .foldable}

> ## MPI_Send and MPI_Recv in Fortran
>
>~~~
> MPI_Send(BUF, COUNT, DATATYPE, DEST, TAG, COMM, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, DEST, TAG, COMM, IERROR
>~~~
>
> | `BUF`:      | Vector containing the data to send |
> | `COUNT`:    | Number of elements to send |
> | `DATATYPE`: | The type of the data being sent |
> | `DEST`:     | The rank number of the rank the data will be sent to |
> | `TAG`:      | A message tag (integer) |
> | `COMM`:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `IERROR`:   | Error status |
>
>~~~
> MPI_Recv(BUF, COUNT, DATATYPE, SOURCE, TAG, COMM, STATUS, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, SOURCE, TAG, COMM
>    INTEGER    STATUS(MPI_STATUS_SIZE), IERROR
>~~~
>
> | `BUF`:      | Vector the received data should be written to             |
> | `COUNT`:    | Maximum number of elements received                       |
> | `DATATYPE`: | The type of the data being received                       |
> | `SOURCE`:   | The rank number of the rank sending the data              |
> | `TAG`:      | A message tag (integer)                                   |
> | `COMM`:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `STATUS`:   | A pointer for writing the exit status of the MPI command  |
>
{: .prereq .foldable}

The number of arguments can make these commands look complicated,
so don't worry if you need to refer back to the documentation regularly
when working with them.
The first four arguments specify what data needs to be sent or received
and the destination or source of the message.

The message tag is used to differentiate messages, in case rank A has sent
multiple pieces of data to rank B.
When rank B requests for a message with the correct tag, the data buffer will
be overwritten by that message.

The communicator is something we have seen before.
It specifies information about the system and where each rank actually is.
The status parameter in `MPI_Recv` will give information about any possible problems
in transit.

> ## Example in C
> ~~~
>#include <stdio.h>
>#include <mpi.h>
>
>int main(int argc, char** argv) {
>  int rank, n_ranks;
>
>  // First call MPI_Init
>  MPI_Init(&argc, &argv);
>
>  // Check that there are two ranks
>  MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
>  if( n_ranks != 2 ){
>    printf("This example requires two ranks\n");
>    MPI_Finalize();
>    return(1);
>  }
>  
>  // Get my rank
>  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
>
>  if( rank == 0 ){
>     char *message = "Hello, world!\n";
>     MPI_Send(message, 14, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
>  }
>
>  if( rank == 1 ){
>     char message[14];
>     MPI_Status  status;
>     MPI_Recv(message, 14, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
>     printf("%s",message);
>  }
>  
>  // Call finalize at the end
>  return MPI_Finalize();
>}
> ~~~
>{: .source .language-c}
>
{: .prereq .foldable}

> ## Example in Fortran
> ~~~
>program hello
>
>     use mpi
>     implicit none
>     
>     integer rank, n_ranks, ierr
>     integer status(MPI_STATUS_SIZE)
>     character(len=13)  message
>
>     ! First call MPI_Init
>     call MPI_Init(ierr)
>
>     ! Check that there are two ranks
>     call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>     if (n_ranks .ne. 2) then
>          write(6,*) "This example requires two ranks"
>          error stop
>     end if
>
>     ! Get my rank
>     call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>
>     if (rank == 0) then
>          message = "Hello, world!"
>          call MPI_Send( message, 13, MPI_CHARACTER, 1, 0, MPI_COMM_WORLD, ierr)
>     end if
>
>     if (rank == 1) then
>          call MPI_Recv( message, 13, MPI_CHARACTER, 0, 0, MPI_COMM_WORLD, status, ierr)
>          write(6,*) message
>     end if
>
>     ! Call MPI_Finalize at the end
>     call MPI_Finalize(ierr)
>end
> ~~~
>{: .source .language-fortran}
>
{: .prereq .foldable}

> ## Try It Out
>
> Compile and run the above code.
>
{: .challenge}


> ## Many Ranks
>
> Change the above example so that it works with any number of ranks.
> Pair even ranks with odd ranks and have each even rank send a message
> to the corresponding odd rank.
>
>> ## Solution in C
>> ~~~
>>#include <stdio.h>
>>#include <mpi.h>
>>
>>int main(int argc, char** argv) {
>>   int rank, n_ranks, my_pair;
>>   
>>   // First call MPI_Init
>>   MPI_Init(&argc, &argv);
>>   
>>   // Get the number of ranks
>>   MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
>>
>>   // Get my rank
>>   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
>>
>>   // Figure out my pair
>>   if( rank%2 == 1 ){
>>      my_pair = rank-1;
>>   } else {
>>      my_pair = rank+1;
>>   }
>>
>>   // Run only if my pair exists
>>   if( my_pair < n_ranks ){
>>
>>      if( rank%2 == 0 ){
>>         char *message = "Hello, world!\n";
>>         MPI_Send(message, 14, MPI_CHAR, my_pair, 0, MPI_COMM_WORLD);
>>      }
>>   
>>      if( rank%2 == 1 ){
>>         char message[14];
>>         MPI_Status  status;
>>         MPI_Recv(message, 14, MPI_CHAR, my_pair, 0, MPI_COMM_WORLD, &status);
>>         printf("%s",message);
>>      }
>>   }
>>
>>   // Call finalize at the end
>>   return MPI_Finalize();
>>}
>> ~~~
>>{: .source .language-c}
>{: .solution}
>
>> ## Solution in Fortran
>> ~~~
>>program hello
>>
>>     use mpi
>>     implicit none
>>     
>>     integer rank, n_ranks, my_pair, ierr
>>     integer status(MPI_STATUS_SIZE)
>>     character(len=13)  message
>>
>>     ! First call MPI_Init
>>     call MPI_Init(ierr)
>>
>>     ! Find the number of ranks
>>     call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>
>>     ! Get my rank
>>     call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>
>>     ! Figure out my pair
>>     if ( MOD(rank,2) == 1 ) then
>>        my_pair = rank-1;
>>     else
>>        my_pair = rank+1;
>>     end if
>>
>>     ! Run only if my pair exists
>>     if( my_pair < n_ranks ) then
>>
>>          if ( MOD(rank,2) == 0 ) then
>>               message = "Hello, world!"
>>               call MPI_Send( message, 13, MPI_CHARACTER, my_pair, 0, MPI_COMM_WORLD, ierr)
>>          end if
>>
>>          if ( MOD(rank,2) == 1 ) then
>>               call MPI_Recv( message, 13, MPI_CHARACTER, my_pair, 0, MPI_COMM_WORLD, status, ierr)
>>               write(6,*) message
>>          end if
>>     end if
>>
>>     ! Call MPI_Finalize at the end
>>     call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .solution}
>
{: .challenge}

> ## Hello Again, World!
>
> Modify the Hello World code so that each rank sends its
> message to rank 0. Have rank 0 print each message.
>
>> ## Hello World in C
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>   int rank;
>>
>>   // First call MPI_Init
>>   MPI_Init(&argc, &argv);
>>
>>   // Get my rank
>>   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>
>>   printf("Hello World, I'm rank %d\n", rank);
>>
>>   // Call finalize at the end
>>   MPI_Finalize();
>> }
>> ~~~
>> {: .source .language-c}
>{: .prereq .foldable}
>
>> ## Hello World in Fortran
>> ~~~
>>program hello
>>
>>    use mpi
>>    implicit none
>>     
>>    integer rank, ierr
>>
>>    ! First call MPI_Init
>>    call MPI_Init(ierr)
>>
>>    ! Get my rank
>>    call MPI_Comm_rank(MPI_COMM_WORLD,rank,ierr)
>>
>>    write(6,*) "Hello World, I'm rank", rank
>>
>>    ! Call MPI_Finalize at the end
>>    call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .prereq .foldable}
>
>> ## Solution in C
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>     int rank, n_ranks, numbers_per_rank;
>>  
>>     // First call MPI_Init
>>     MPI_Init(&argc, &argv);
>>     // Get my rank and the number of ranks
>>     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>     MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>  
>>     if( rank != 0 ){
>>        // All ranks other than 0 should send a message
>>  
>>        char message[20];
>>        sprintf(message, "Hello World, I'm rank %d\n", rank);
>>        MPI_Send(message, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
>>  
>>     } else {
>>        // Rank 0 will receive each message and print them
>>  
>>        for( int sender=1; sender<n_ranks; r++ ){
>>           char message[20];
>>           MPI_Status status;
>>  
>>           MPI_Recv(message, 13, MPI_CHAR, sender, 0, MPI_COMM_WORLD, &status);
>>           printf("%s",message);
>>        }
>>     }
>>     
>>     // Call finalize at the end
>>     MPI_Finalize();
>> }
>> ~~~
>>{: .source .language-c}
>{: .solution}
>
>> ## Solution in Fortran
>> ~~~
>>program hello
>>
>>    use mpi
>>    implicit none
>>     
>>    integer rank, n_ranks, ierr
>>    integer sender
>>    integer status(MPI_STATUS_SIZE)
>>    character(len=40) message
>>
>>    ! First call MPI_Init
>>    call MPI_Init(ierr)
>>
>>    ! Get my rank and the number of ranks
>>    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>    call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>
>>    if (rank .NE. 0) then
>>        ! All ranks other than 0 should send a message
>>
>>        write(message,*) "Hello World, I'm rank", rank
>>        call MPI_Send( message, 40, MPI_CHARACTER, 0, 0, MPI_COMM_WORLD, ierr)
>>
>>    else
>>        ! Rank 0 will receive each message and print them
>>
>>        do sender = 1, n_ranks-1
>>            call MPI_Recv( message, 40, MPI_CHARACTER, sender, 0, MPI_COMM_WORLD, status, ierr)
>>            write(6,*) message
>>        end do
>>
>>    end if
>>
>>    ! Call MPI_Finalize at the end
>>    call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .solution}
{: .challenge}



> ## Blocking
>
> * Try this code and see what happens
> * How would you change the code to fix the problem?
>
>> ## C
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>    int rank, n_ranks, neighbour;
>>    int n_numbers = 524288;
>>    int send_message[n_numbers];
>>    int recv_message[n_numbers];
>>    MPI_Status status;
>> 
>>    // First call MPI_Init
>>    MPI_Init(&argc, &argv);
>> 
>>    // Get my rank and the number of ranks
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>>
>>    // Check that there are exactly two ranks
>>    if( n_ranks != 2 ){
>>         printf("This example requires exactly two ranks\n");
>>         MPI_Finalize();
>>         return(1);
>>    }
>> 
>>    // Call the other rank the neighbour
>>    if( rank == 0 ){
>>       neighbour = 1;      
>>    } else {
>>       neighbour = 0;
>>    }
>> 
>>    // Generate numbers to send
>>    for( int i=0; i<n_numbers; i++){
>>       send_message[i] = i;
>>    }
>> 
>>    // Send the message to other rank
>>    MPI_Send(send_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD);
>> 
>>    // Receive the message from the other rank
>>    MPI_Recv(recv_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &status);
>>    printf("Message received by rank %d \n", rank);
>> 
>>    // Call finalize at the end
>>    MPI_Finalize();
>> }
>> ~~~
>>{: .source .language-c}
>{: .prereq .foldable}
>
>
>> ## Fortran
>> ~~~
>>program hello
>>
>>    use mpi
>>    implicit none
>>     
>>    integer, parameter :: n_numbers=524288
>>    integer i
>>    integer rank, n_ranks, neighbour, ierr
>>    integer status(MPI_STATUS_SIZE)
>>    integer send_message(n_numbers)
>>    integer recv_message(n_numbers)
>>
>>    ! First call MPI_Init
>>    call MPI_Init(ierr)
>>
>>    ! Get my rank and the number of ranks
>>    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>    call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>
>>    ! Check that there are exactly two ranks
>>    if (n_ranks .NE. 2) then
>>         write(6,*) "This example requires exactly two ranks"
>>         error stop
>>    end if
>>
>>    ! Call the other rank the neighbour
>>    if (rank == 0) then
>>        neighbour = 1
>>    else
>>        neighbour = 0
>>    end if
>>
>>    ! Generate numbers to send
>>    do i = 1, n_numbers
>>        send_message(i) = i;
>>    end do
>>
>>    ! Send the message to other rank
>>    call MPI_Send( send_message, n_numbers, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, ierr )
>>
>>    ! Receive the message from the other rank
>>    call MPI_Recv( recv_message, n_numbers, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, status, ierr )
>>    write(6,*) "Message received by rank", rank
>>
>>    ! Call MPI_Finalize at the end
>>    call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .prereq .foldable}
>
>
>> ## Solution in C
>> 
>> `MPI_Send` will block execution until until the receiving process has called
>> `MPI_Recv`. This prevents the sender from unintentionally modifying the message
>> buffer before the message is actually sent.
>> Above, both ranks call `MPI_Send` and just wait for the other respond.
>> The solution is to have one of the ranks receive it's message before sending.
>>
>> Sometimes `MPI_Send` will actually make a copy of the buffer and return immediately.
>> This generally happens only for short messages.
>> Even when this happens, the actual transfer will not start before the receive is posted.
>> 
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>    int rank, n_ranks, neighbour;
>>    int n_numbers = 524288;
>>    int send_message[n_numbers];
>>    int recv_message[n_numbers];
>>    MPI_Status status;
>> 
>>    // First call MPI_Init
>>    MPI_Init(&argc, &argv);
>> 
>>    // Get my rank and the number of ranks
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
>> 
>>    // Generate numbers to send
>>    for( int i=0; i<n_numbers; i++){
>>       send_message[i] = i;
>>    }
>> 
>>    if( rank == 0 ){
>>       // Rank 0 will send first
>>       MPI_Send(send_message, n_numbers, MPI_INT, 1, 0, MPI_COMM_WORLD);
>>    }
>> 
>>    if( rank == 1 ){
>>       // Rank 1 will receive it's message before sending
>>       MPI_Recv(recv_message, n_numbers, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
>>       printf("Message received by rank %d \n", rank);
>>    }
>> 
>>    if( rank == 1 ){
>>       // Now rank 1 is free to send
>>       MPI_Send(send_message, n_numbers, MPI_INT, 0, 0, MPI_COMM_WORLD);
>>    }
>> 
>>    if( rank == 0 ){
>>       // And rank 0 will receive the message
>>       MPI_Recv(recv_message, n_numbers, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
>>       printf("Message received by rank %d \n", rank);
>>    }
>> 
>>    // Call finalize at the end
>>    MPI_Finalize();
>> }
>> ~~~
>>{: .source .language-c}
>{: .solution}
>
>
>> ## Solution in Fortran
>> 
>> `MPI_Send` will block execution until until the receiving process has called
>> `MPI_Recv`. This prevents the sender from unintentionally modifying the message
>> buffer before the message is actually sent.
>> Above, both ranks call `MPI_Send` and just wait for the other respond.
>> The solution is to have one of the ranks receive its message before sending.
>>
>> Sometimes `MPI_Send` will actually make a copy of the buffer and return immediately.
>> This generally happens only for short messages.
>> Even when this happens, the actual transfer will not start before the receive is posted.
>> 
>> ~~~
>>program hello
>>
>>   use mpi
>>   implicit none
>>    
>>   integer, parameter :: n_numbers=524288
>>   integer i
>>   integer rank, n_ranks, neighbour, ierr
>>   integer status(MPI_STATUS_SIZE)
>>   integer send_message(n_numbers)
>>   integer recv_message(n_numbers)
>>
>>   ! First call MPI_Init
>>   call MPI_Init(ierr)
>>
>>   ! Get my rank and the number of ranks
>>   call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>   call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>
>>   ! Check that there are exactly two ranks
>>   if (n_ranks .NE. 2) then
>>        write(6,*) "This example requires exactly two ranks"
>>        error stop
>>   end if
>>
>>   ! Generate numbers to send
>>   do i = 1, n_numbers
>>       send_message(i) = i;
>>   end do
>>
>>   if (rank == 0) then
>>     ! Rank 0 will send first
>>     call MPI_Send( send_message, n_numbers, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, ierr )
>>   end if
>>
>>   if (rank == 1) then
>>     ! Receive the message from the other rank
>>     call MPI_Recv( recv_message, n_numbers, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, status, ierr )
>>     write(6,*) "Message received by rank", rank
>>   end if
>>
>>   if (rank == 1) then
>>     ! Rank 1 will send second
>>     call MPI_Send( send_message, n_numbers, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, ierr )
>>   end if
>>
>>   if (rank == 0) then
>>     ! Receive the message from the other rank
>>     call MPI_Recv( recv_message, n_numbers, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, status, ierr )
>>     write(6,*) "Message received by rank", rank
>>   end if
>>
>>
>>   ! Call MPI_Finalize at the end
>>   call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
>{: .solution}
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
>> ## Solution in C
>> 
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>    int rank, neighbour;
>>    int max_count = 1000000;
>>    int counter;
>>    int bored;
>>    int ball = 1; // A dummy message to simulate the ball
>>    MPI_Status status;
>> 
>>    // First call MPI_Init
>>    MPI_Init(&argc, &argv);
>> 
>>    // Get my rank
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>> 
>>    // Call the other rank the neighbour
>>    if( rank == 0 ){
>>        neighbour = 1;
>>    } else {
>>        neighbour = 0;
>>    }
>> 
>>    if( rank == 0 ){
>>       // Rank 0 starts with the ball. Send it to rank 1
>>       MPI_Send(&ball, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
>>    }
>> 
>>    // Now run a send and receive in a loop untill someone gets bored
>>    counter = 0;
>>    bored = 0;
>>    while( !bored )
>>    {
>>       // Receive the ball
>>       MPI_Recv(&ball, 1, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &status);
>>       
>>       // Increment the counter and send the ball back
>>       MPI_Send(&ball, 1, MPI_INT, neighbour, 0, MPI_COMM_WORLD);
>> 
>>       // Check if the rank is bored
>>       bored = counter < max_count;
>>    }
>>    printf("rank %d is bored and giving up \n", rank);
>> 
>>    // Call finalize at the end
>>    MPI_Finalize();
>> }
>> ~~~
>>{: .source .language-c}
> {: .solution}
>
>
>> ## Solution in Fortran
>> 
>> ~~~
>>program pingpong
>>
>>    use mpi
>>    implicit none
>>     
>>    integer ball, max_count, counter
>>    logical bored
>>    integer rank, neighbour, ierr
>>    integer status(MPI_STATUS_SIZE)
>>
>>    ball = 1 ! A dummy message to simulate the ball
>>    max_count = 1000000
>>
>>    ! First call MPI_Init
>>    call MPI_Init(ierr)
>>
>>    ! Get my rank
>>    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>>
>>    ! Call the other rank the neighbour
>>    if (rank == 0) then
>>        neighbour = 1
>>    else
>>        neighbour = 0
>>    end if
>>
>>    ! Rank 0 starts with the ball. Send it to rank 1.
>>    if ( rank == 0 ) then
>>        call MPI_Send( ball, 1, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, ierr )
>>    end if
>>
>>    ! Now run send and receive in a loop until someone gets bored
>>    counter = 0
>>    bored = .false.
>>    do while ( .NOT. bored )
>>        ! Receive the ball
>>        call MPI_Recv( ball, 1, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, status, ierr )
>>
>>        ! Increment the counter and send the ball back
>>        counter = counter + 1
>>        call MPI_Send( ball, 1, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, ierr )
>>
>>        ! Check if the rank is bored
>>        bored = counter < max_count
>>    end do
>>
>>    write(6, *) "Rank ", rank, "is bored and giving up"
>>
>>    ! Call MPI_Finalize at the end
>>    call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-fortran}
> {: .solution}
>
>
{: .challenge}



{% include links.md %}

