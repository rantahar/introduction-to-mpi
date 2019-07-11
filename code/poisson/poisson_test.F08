module poisson_test
  use fruit
  use poisson_solver
  implicit none
  include "mpif.h" 

contains

   subroutine test_poisson_step
      integer, parameter :: MAX=20

      real u(0:(MAX+1),0:(MAX+1)), unew(0:(MAX+1),0:(MAX+1))
      real rho(0:(MAX+1),0:(MAX+1))
      real h, hsq, difference
      double precision unorm, residual
      integer i, j, rank, n_ranks, ierr
      integer my_j_max

      ! Get my rank and the number of ranks
      call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
      call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
   
      ! Find the number of x-slices calculated by each rank
      ! The simple calculation here assumes that MAX is divisible by n_ranks
      my_j_max = MAX/n_ranks;

      ! Run setup
      hsq = h*h

      ! Initialise the u and rho field to 0 
      do j = 0, my_j_max+1
         do i = 0, MAX+1
            u(i,j) = 0.0
            rho(i,j) = 0.0
         enddo
      enddo

      ! Test a configuration with u=10 at x=1 and y=1
      ! The coordinate x=1, y=1 is always in rank 0
      if (rank == 0) then
         u(1,1) = 10
      end if

      ! Run a single iteration of the poisson solver
      call poisson_step( u, unew, rho, MAX, hsq, unorm )
      call assert_true( unorm == 112.5, "Test One Step")

      ! Run a single iteration of the poisson solver
      do i = 1, 50
         call poisson_step( u, unew, rho, MAX, hsq, unorm )
      end do

      write(6,*) unorm
      difference = unorm - 0.0018388170223
      call assert_true( difference*difference < 1e-16, "Test 50 Steps")

   end subroutine test_poisson_step

end module poisson_test