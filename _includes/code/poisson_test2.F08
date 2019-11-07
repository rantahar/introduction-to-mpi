>>program poisson
>>
>>   implicit none
>>   include "mpif.h" 
>>
>>   integer, parameter :: GRIDSIZE=10
>>   ! We don't know the number of ranks yet, so we
>>   ! allocate these arrays later
>>   real, allocatable :: u(:), unew(:)
>>   real, allocatable :: rho(:)
>>   integer i
>>   integer my_rank, n_ranks, ierr
>>   integer my_j_max
>>
>>   real h, hsq
>>   double precision unorm, difference
>>
>>   ! Initialize MPI
>>   call MPI_Init(ierr)
>>
>>   ! Get my rank and the number of ranks
>>   call MPI_Comm_rank(MPI_COMM_WORLD, my_rank, ierr)
>>   call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>
>>   ! Find the number of x-slices calculated by each rank
>>   ! The simple calculation here assumes that GRIDSIZE is divisible by n_ranks
>>   my_j_max = GRIDSIZE/n_ranks;
>>
>>   ! Now allocate the fields
>>   real u(0:(my_j_max+1)), unew(0:(my_j_max+1))
>>   real rho(0:(my_j_max+1))
>>
>>   ! Set up parameters
>>   h = 0.1
>>   hsq = h*h
>>
>>   ! Initialise the u and rho field to 0 
>>   do i = 0, my_j_max+1
>>       u(i) = 0.0
>>       rho(i) = 0.0
>>   enddo
>>
>>   ! Test a configuration with u=10 at x=0 boundary
>>   if (my_rank == 0) then	
>>     u(0) = 10
>>   end if
>>
>>   ! Run a single iteration of the poisson solver
>>   call poisson_step( u, unew, rho, my_j_max, hsq, unorm )
>>   
>>   if (unorm == 25) then
>>      write(6,*) "PASSED after 1 step"
>>   else
>>      write(6,*) "FAILED after 1 step"
>>      write(6,*) unorm
>>   end if
>>
>>   ! Run 9 more iterations for a total of 10
>>   do i = 1, 10
>>      call poisson_step( u, unew, rho, my_j_max, hsq, unorm )
>>   end do
>>
>>   difference = unorm - 0.40042400360107422
>>   if (difference*difference < 1e-16) then
>>      write(6,*) "PASSED after 10 steps"
>>   else
>>      write(6,*) "FAILED after 10 steps"
>>      write(6,*) unorm
>>   end if
>>
>>   ! Call MPI_Finalize at the end
>>   call MPI_Finalize(ierr)
>>
>>end