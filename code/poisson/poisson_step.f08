! a serial code for Poisson equation 
! contact seyong.kim81@gmail.com for comments and questions 

module poisson

  implicit none

contains

   subroutine poisson_step(u, unew, rho, MAX, hsq, unorm)
      integer, intent(in) :: MAX
      real, intent(inout), dimension (0:(MAX+1),0:(MAX+1)) :: u, unew
      real, intent(in), dimension (0:(MAX+1),0:(MAX+1)) :: rho
      real hsq
      double precision, intent(out) :: unorm
      integer i, j

      ! Calculate one timestep
      do j = 1, MAX
         do i = 1, MAX
            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
         enddo
      enddo

      ! Find the difference compared to the previous time step
      unorm = 0.0
      do j = 1, MAX
         do i = 1, MAX
            unorm = unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
         enddo
      enddo

      write(6,*) 'unorm = ', unorm

      do j = 1, MAX
         do i = 1, MAX
            u(i,j) = unew(i,j)
         enddo
      enddo

   end subroutine poisson_step

end module


