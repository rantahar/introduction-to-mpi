>>program poisson
>>
>>   implicit none
>>   include "mpif.h" 
>>
>>   integer, parameter :: GRIDSIZE=20
>>   integer i,j
>>   integer my_rank, n_ranks, ierr
>>   integer my_j_max
>>
>>   real u(0:(GRIDSIZE+1),0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
>>   real rho(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
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
>>   ! Set up parameters
>>   h = 0.1
>>   hsq = h*h
>>
>>   ! Initialise the u and rho field to 0 
>>   do j = 0, my_j_max+1
>>      do i = 0, GRIDSIZE+1
>>         u(i,j) = 0.0
>>         rho(i,j) = 0.0
>>      enddo
>>   enddo
>>
>>   ! Test a configuration with u=10 at x=1 and y=1
>>   ! The coordinate x=1, y=1 is always in rank 0
>>   if (my_rank == 0) then
>>     u(1,1) = 10
>>   end if
>>
>>   ! Run a single iteration of the poisson solver
>>   call poisson_step( u, unew, rho, GRIDSIZE, hsq, unorm )
>>   
>>   if (unorm == 112.5) then
>>      write(6,*) "PASSED after 1 step"
>>   else
>>      write(6,*) "FAILED after 1 step"
>>   end if
>>
>>   ! Run a single iteration of the poisson solver
>>   do i = 1, 50
>>      call poisson_step( u, unew, rho, GRIDSIZE, hsq, unorm )
>>   end do
>>
>>   difference = unorm - 0.0018388170223
>>   if (difference*difference < 1e-16) then
>>      write(6,*) "PASSED after 50 steps"
>>   else
>>      write(6,*) "FAILED after 50 steps"
>>   end if
>>
>>   ! Call MPI_Finalize at the end
>>   call MPI_Finalize(ierr)
>>
>>end