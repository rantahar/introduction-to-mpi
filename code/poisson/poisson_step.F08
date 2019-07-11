module poisson_solver

   implicit none
   include "mpif.h" 

contains

   subroutine poisson_step(u, unew, rho, MAX, hsq, unorm)
      integer, intent(in) :: MAX
      real, intent(inout), dimension (0:(MAX+1),0:(MAX+1)) :: u, unew
      real, intent(in), dimension (0:(MAX+1),0:(MAX+1)) :: rho
      real, intent(in) :: hsq
      double precision local_unorm
      double precision, intent(out) :: unorm
      integer status(MPI_STATUS_SIZE)
      integer my_j_max, n_ranks, rank
      integer ierr, i, j
      
      ! Find the number of x-slices calculated by each rank
      ! The simple calculation here assumes that MAX is divisible by n_ranks
      call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
      my_j_max = MAX/n_ranks

      ! We need the rank number for the nearest neighbour communication
      call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

      ! Calculate one timestep
      do j = 1, my_j_max
         do i = 1, MAX
            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
         enddo
      enddo

      ! Find the difference compared to the previous time step
      local_unorm = 0.0
      do j = 1, my_j_max
         do i = 1, MAX
            local_unorm = local_unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
         enddo
      enddo

      call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )

      ! Overwrite u with the new field
      do j = 1, my_j_max
         do i = 1, MAX
            u(i,j) = unew(i,j)
         enddo
      enddo

      ! The u field has been changed, communicate it to neighbours
      ! With blocking communication, half the ranks should send first
      ! and the other half should receive first
      if (mod(rank,2) == 1) then
         ! Ranks with odd number send first

         ! Send data down from rank to rank-1
         call MPI_Send( unew(1,1), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, ierr)
         ! Receive dat from rank-1
         call MPI_Recv( u(1,0), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, status, ierr)

         if (rank < (n_ranks-1)) then
            call MPI_Send( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, ierr)
            call MPI_Recv( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, status, ierr)
         endif

      else
         ! Ranks with even number receive first

         if (rank > 0) then
            call MPI_Recv( u(1,0), MAX, MPI_REAL, rank-1, 1, MPI_COMM_WORLD, status, ierr)
            call MPI_Send( unew(1,1), MAX, MPI_REAL, rank-1, 2, MPI_COMM_WORLD, ierr)
         endif

         if (rank < (n_ranks-1)) then
            call MPI_Recv( u(1,my_j_max+1), MAX, MPI_REAL, rank+1, 1, MPI_COMM_WORLD, status, ierr)
            call MPI_Send( unew(1,my_j_max), MAX, MPI_REAL, rank+1, 2, MPI_COMM_WORLD, ierr)
         endif
      endif

   end subroutine poisson_step

end module


