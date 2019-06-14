! a serial code for Poisson equation 
! contact seyong.kim81@gmail.com for comments and questions 
      
program poisson

   implicit none

   integer, parameter :: GRIDSIZE=100
   integer i,j

   real u(0:(GRIDSIZE+1),0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
   real rho(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
   real h, hsq
   double precision unorm, residual

   ! Set up parameters
   h = 0.1
   hsq = h*h
   residual = 1e-5

   ! Initialise the u and rho field to 0 
   do j = 0, GRIDSIZE+1
      do i = 0, GRIDSIZE+1
         u(i,j) = 0.0
         rho(i,j) = 0.0
      enddo
   enddo

   ! Create a start configuration with the field
   ! u=10 at x=0
   do j = 0, GRIDSIZE+1
      u(0,j) = 10.0
   enddo

   ! Run iterations until the field reaches an equilibrium
   do 

      call poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)

      if ( sqrt(unorm) <= sqrt(residual) ) then
         exit
      end if

   enddo

   write(6,*) 'Run completed with residue ', unorm

end

subroutine poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)

   implicit none

   integer, intent(in) :: GRIDSIZE
   real, intent(inout), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: u, unew
   real, intent(in), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: rho
   real, intent(in) :: hsq
   double precision, intent(out) :: unorm
   integer i, j
   
   ! Calculate one timestep
   do j = 1, GRIDSIZE
      do i = 1, GRIDSIZE
         unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1)- hsq*rho(i,j))
      enddo
   enddo

   ! Find the difference compared to the previous time step
   unorm = 0.0
   do j = 1, GRIDSIZE
      do i = 1, GRIDSIZE
         unorm = unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
      enddo
   enddo

   do j = 1, GRIDSIZE
      do i = 1, GRIDSIZE
         u(i,j) = unew(i,j)
      enddo
   enddo
end