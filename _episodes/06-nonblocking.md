---
title: "Non-blocking Communication"
teaching: 10
exercises: 10
questions:
- "How do I interleave communication and computation?"
objectives:
- "Introduce `MPI_ISend`, `MPI_IRecv`, `MPI_Test` and `MPI_Wait`"
keypoints:
- "Non-blocking functions allows interleaving communication and computation"
---

## Non-Blocking Communication

### Send and Receive

In one of the previous lessons we used the `MPI_Send` and `MPI_Recv` functions
to communicate between the ranks.
We saw that these functions are blocking:
`MPI_Send` will only return when the program can safely modify the send buffer and
`MPI_Recv` will only return once the data has been received and
written to the receive buffer.
This is safe and usually straightforward, but causes the program to wait
while the communication is happening.
Usually there is computation that we could perform while waiting for data.

The MPI standard includes non-blocking versions of the send and receive functions,
`MPI_ISend` and `MPI_IRecv`.
These function will return immediately, giving you more control of the flow
of the program. After calling them, it is not safe to modify the sending or
the receiving buffer, but the program is free to continue with other operations.
When it needs the data in the buffers, it needs to make sure the communication process
is complete using the `MPI_Wait` and `MPI_Test` functions.

> ## `MPI_ISend` and `MPI_IRecv` in C
>
>~~~
> MPI_ISend(
>    void* data,
>    int count,
>    MPI_Datatype datatype,
>    int destination,
>    int tag,
>    MPI_Comm communicator,
>    MPI_Request* request)
>~~~
>
> | `data`:         | Pointer to the start of the data being sent |
> | `count`:        | Number of elements to send |
> | `datatype`:     | The type of the data being sent |
> | `destination`:  | The rank number of the rank the data will be sent to |
> | `tag`:          | A message tag (integer) |
> | `communicator`: | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `request`:      | Pointer for writing the request structure |
>
>~~~
> MPI_IRecv(
>    void* data,
>    int count,
>    MPI_Datatype datatype,
>    int source,
>    int tag,
>    MPI_Comm communicator,
>    MPI_Request* request)
>~~~
>
> | `data`:         | Pointer to where the received data should be written |
> | `count`:        | Maximum number of elements received |
> | `datatype`:     | The type of the data being received |
> | `source`:       | The rank number of the rank sending the data |
> | `tag`:          | A message tag (integer) |
> | `communicator`: | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `request`:      | Pointer for writing the request structure |
>
{: .prereq .foldable}

> ## `MPI_ISend` and `MPI_IRecv` in Fortran
>
>~~~
> MPI_ISend(BUF, COUNT, DATATYPE, DEST, TAG, COMM, REQUEST, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, DEST, TAG, COMM, REQUEST, IERROR
>~~~
>
> | `BUF`:      | Vector containing the data to send |
> | `COUNT`:    | Number of elements to send |
> | `DATATYPE`: | The type of the data being sent |
> | `DEST`:     | The rank number of the rank the data will be sent to |
> | `TAG`:      | A message tag (integer) |
> | `COMM`:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `REQUEST`:  | Request handle |
> | `IERROR`:   | Error status |
>
>~~~
> MPI_IRecv(BUF, COUNT, DATATYPE, SOURCE, TAG, COMM, REQUEST, IERROR)
>    <type>    BUF(*)
>    INTEGER    COUNT, DATATYPE, SOURCE, TAG, COMM,
>    INTEGER    REQUEST, IERROR
>~~~
>
> | `BUF`:      | Vector the received data should be written to             |
> | `COUNT`:    | Maximum number of elements received                       |
> | `DATATYPE`: | The type of the data being received                       |
> | `SOURCE`:   | The rank number of the rank sending the data              |
> | `TAG`:      | A message tag (integer)                                   |
> | `COMM`:     | The communicator (we have used MPI_COMM_WORLD in earlier) |
> | `REQUEST`:  | Request handle                                            |
> | `IERROR`:   | Error status |
>
{: .prereq .foldable}

There's one new parameter here, a request.
This is used to keep track of each separate transfer started by the program.
You can use it to check the status of a transfer using the `MPI_Test` function,
or call `MPI_Wait` to wait until the transfer is complete.


### Test and Wait

`MPI_Test` will return the status of the transfer specified by a request and
`MPI_Wait` will wait until the transfer is complete before returning.
The request can be created by either an `MPI_ISend` or an `MPI_IRecv`.

> ## `MPI_Test` and `MPI_Wait` in C
>
>~~~
> MPI_Test(
>    MPI_Request* request,
>    int * flag,
>    MPI_Status* status)
>~~~
>
> | `request`:      | The request |
> | `flag`:         | Pointer for writing the result of the test |
> | `status`:       | A pointer for writing the exit status of the MPI command |
>
>~~~
> MPI_Wait(
>    MPI_Request* request,
>    MPI_Status* status)
>~~~
>
> | `request`:      | The request |
> | `status`:       | A pointer for writing the exit status of the MPI command |
>
{: .prereq .foldable}

> ## `MPI_Test` and `MPI_Wait` in Fortran
>
>~~~
> MPI_TEST(REQUEST, FLAG, STATUS, IERROR)
>    LOGICAL    FLAG
>    INTEGER    REQUEST, STATUS(MPI_STATUS_SIZE), IERROR
>~~~
>
> | `REQUEST`:  | The request |
> | `FLAG`:     | Pointer for writing the result of the test |
> | `STATUS`:   | A pointer for writing the exit status of the MPI command |
> | `IERROR`:   | Error status |
>
>~~~
>MPI_WAIT(REQUEST, STATUS, IERROR)
>    INTEGER    REQUEST, STATUS(MPI_STATUS_SIZE), IERROR
>~~~
>
> | `REQUEST`:  | The request |
> | `STATUS`:   | A pointer for writing the exit status of the MPI command |
> | `IERROR`:   | Error status |
>
{: .prereq .foldable}


### Examples

These functions can be used similarly to `MPI_Send` and `MPI_Recv`.

> ## Example in C
> ~~~
> #include <stdio.h>
> #include <mpi.h>
>
> main(int argc, char** argv) {
>   int rank, n_ranks;
>   int my_first, my_last;
>   int numbers = 10;
>
>   // First call MPI_Init
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
>      MPI_ISend(message, 13, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
>   }
>
>   if( rank == 1 ){
>      char message[13];
>      int status;
>      MPI_IRecv(message, 13, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
>      MPI_Wait( &request, &status );
>      printf("%s",message)
>   }
>   
>   // Call finalize at the end
>   MPI_Finalize();
> }
> ~~~
>{: .source .language-c}
{: .prereq .foldable}


> ## Example in Fortran
> ~~~
>program hello
>
>     use mpi
>     implicit none
>     
>     integer rank, n_ranks, request, ierr
>     integer status(MPI_STATUS_SIZE)
>     character(len=13)  message
>
>     ! First call MPI_Init
>     call MPI_Init(ierr)
>
>     ! Check that there are at least two ranks
>     call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>     if (n_ranks < 2) then
>          write(6,*) "This example requires at least two ranks"
>          error stop
>     end if
>
>     ! Get my rank
>     call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
>
>     if (rank == 0) then
>          message = "Hello, world!"
>          call MPI_ISend( message, 13, MPI_CHARACTER, 1, 0, MPI_COMM_WORLD, request, ierr )
>     end if
>
>     if (rank == 1) then
>          call MPI_IRecv( message, 13, MPI_CHARACTER, 0, 0, MPI_COMM_WORLD, request, ierr )
>          call MPI_WAIT( request, status, ierr )
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



> ## Non-Blocking Communication
>
> Here is the blocking example again.
> Fix the problem using `MPI_ISend`, `MPI_IRecv` and `MPI_Wait`.
>
>> ## C
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>    int rank, n_ranks, neighbour;
>>    int n_numbers = 1048576;
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
>> {: .source .language-c}
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
>>    integer, parameter :: n_numbers=1048576
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
> {: .prereq .foldable}
>
>
>> ## Solution in C
>> 
>> ~~~
>> #include <stdio.h>
>> #include <mpi.h>
>> 
>> int main(int argc, char** argv) {
>>    int rank, n_ranks, neighbour;
>>    int n_numbers = 1048576;
>>    int send_message[n_numbers];
>>    int recv_message[n_numbers];
>>    MPI_Status status;
>>    MPI_Request* request;
>> 
>>    // First call MPI_Init
>>    MPI_Init(&argc, &argv);
>> 
>>    // Get my rank and the number of ranks
>>    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>>    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
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
>>    MPI_ISend(send_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &request);
>> 
>>    // Receive the message from the other rank
>>    MPI_IRecv(recv_message, n_numbers, MPI_INT, neighbour, 0, MPI_COMM_WORLD, &request);
>>    MPI_Wait( &request, &status );
>>    printf("Message received by rank %d \n", rank);
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
>> ~~~
>>program hello
>>
>>   use mpi
>>   implicit none
>>    
>>   integer, parameter :: n_numbers=1048576
>>   integer i
>>   integer rank, n_ranks, neighbour, request, ierr
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
>>   ! Call the other rank the neighbour
>>   if (rank == 0) then
>>       neighbour = 1
>>   else
>>       neighbour = 0
>>   end if
>>
>>   ! Generate numbers to send
>>   do i = 1, n_numbers
>>       send_message(i) = i;
>>   end do
>>
>>   ! Send the message to other rank
>>   call MPI_ISend( send_message, n_numbers, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, request, ierr )
>>
>>   ! Receive the message from the other rank
>>   call MPI_IRecv( recv_message, n_numbers, MPI_INTEGER, neighbour, 0, MPI_COMM_WORLD, request, ierr )
>>   call MPI_WAIT( request, status, ierr )
>>   write(6,*) "Message received by rank", rank
>>
>>   ! Call MPI_Finalize at the end
>>   call MPI_Finalize(ierr)
>>end
>> ~~~
>>{: .source .language-c}
>{: .solution}
>
>
{: .challenge}



{% include links.md %}

