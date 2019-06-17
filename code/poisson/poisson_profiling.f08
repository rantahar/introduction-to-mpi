program poisson

  implicit none
  include "mpif.h"

  integer, parameter :: GRIDSIZE=512
  integer i,j
  integer my_rank, n_ranks, ierr
  integer my_j_max

  real u(0:(GRIDSIZE+1),0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
  real rho(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
  real h, hsq
  double precision unorm, difference

  ! Initialize MPI
  call MPI_Init(ierr)

  ! Get my rank and the number of ranks
  call MPI_Comm_rank(MPI_COMM_WORLD,my_rank,ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)

  ! Find the number of x-slices calculated by each rank
  ! The simple calculation here assumes that MAX is divisible by n_ranks
  my_j_max = GRIDSIZE/n_ranks;

  ! Set up parameters
  h = 0.1
  hsq = h*h

  ! Initialise the u and rho field to 0
  do j = 0, my_j_max+1
     do i = 0, GRIDSIZE+1
        u(i,j) = 0.0
        rho(i,j) = 0.0
     enddo
  enddo

  ! Test a configuration with u=10 at x=1 and y=1
  ! The coordinate x=1, y=1 is always in rank 0
  if (my_rank == 0) then
    u(1,1) = 10
  end if

  ! Run 100 steps
  do i = 1, 100
     call poisson_step( u, unew, rho, GRIDSIZE, hsq, unorm )
  end do

  write(6,*) "Run completed with residue", unorm

  ! Call MPI_Finalize at the end
  call MPI_Finalize(ierr)

end


subroutine poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)

   implicit none
   include "mpif.h"

   integer, intent(in) :: GRIDSIZE
   real, intent(inout), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: u, unew
   real, intent(in), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: rho
   real, intent(in) :: hsq
   double precision local_unorm
   double precision, intent(out) :: unorm
   integer status(MPI_STATUS_SIZE)
   integer my_j_max, n_ranks, my_rank
   integer ierr, i, j

   ! Find the number of x-slices calculated by each rank
   ! The simple calculation here assumes that GRIDSIZE is divisible by n_ranks
   call MPI_Comm_rank(MPI_COMM_WORLD, my_rank, ierr)
   call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
   my_j_max = GRIDSIZE/n_ranks

   ! Calculate one timestep
   do j = 1, my_j_max
      do i = 1, GRIDSIZE
         unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
      enddo
   enddo

   ! Find the difference compared to the previous time step
   local_unorm = 0.0
   do j = 1, my_j_max
      do i = 1, GRIDSIZE
         local_unorm = local_unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
      enddo
   enddo

   call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )

   ! Overwrite u with the new field
   do j = 1, my_j_max
      do i = 1, GRIDSIZE
         u(i,j) = unew(i,j)
      enddo
   enddo

  ! The u field has been changed, communicate it to neighbours
       ! With blocking communication, half the ranks should send first
       ! and the other half should receive first
   if (mod(my_rank,2) == 1) then
      ! Ranks with odd number send first

      ! Send data down from my_rank to my_rank-1
      call MPI_Send( unew(1,1), GRIDSIZE, MPI_REAL, my_rank-1, 1, MPI_COMM_WORLD, ierr)
      ! Receive dat from my_rank-1
      call MPI_Recv( u(1,0), GRIDSIZE, MPI_REAL, my_rank-1, 2, MPI_COMM_WORLD, status, ierr)

      if (my_rank < (n_ranks-1)) then
         call MPI_Send( unew(1,my_j_max), GRIDSIZE, MPI_REAL, my_rank+1, 1, MPI_COMM_WORLD, ierr)
         call MPI_Recv( u(1,my_j_max+1), GRIDSIZE, MPI_REAL, my_rank+1, 2, MPI_COMM_WORLD, status, ierr)
      endif

  else
      ! Ranks with even number receive first

      if (my_rank > 0) then
         call MPI_Recv( u(1,0), GRIDSIZE, MPI_REAL, my_rank-1, 1, MPI_COMM_WORLD, status, ierr)
         call MPI_Send( unew(1,1), GRIDSIZE, MPI_REAL, my_rank-1, 2, MPI_COMM_WORLD, ierr)
      endif

      if (my_rank < (n_ranks-1)) then
         call MPI_Recv( u(1,my_j_max+1), GRIDSIZE, MPI_REAL, my_rank+1, 1, MPI_COMM_WORLD, status, ierr)
         call MPI_Send( unew(1,my_j_max), GRIDSIZE, MPI_REAL, my_rank+1, 2, MPI_COMM_WORLD, ierr)
      endif
  endif

end subroutine poisson_step
