! A serial code for Poisson equation 
! This will apply the diffusion equation to an initial state
! untill an equilibrium state is reached. 

! contact seyong.kim81@gmail.com for comments and questions 
      
program poisson

   implicit none

   integer, parameter :: GRIDSIZE=10
   integer i

   real u(0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1))
   real rho(0:(GRIDSIZE+1))
   real h, hsq
   double precision unorm, residual

   ! Set up parameters
   h = 0.1
   hsq = h*h
   residual = 1e-5

   ! Initialise the u and rho field to 0 
   do i = 0, GRIDSIZE+1
      u(i) = 0.0
      rho(i) = 0.0
   enddo

   ! Create a start configuration with the field
   ! u=10 at the x=0 boundary
   u(0) = 10.0

   ! Run iterations until the field reaches an equilibrium and
   ! no longer changes
   do 

      call poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)
      write(6,*) 'Iteration', i, ', residue', unorm

      if ( sqrt(unorm) <= sqrt(residual) ) then
         exit
      end if

   enddo

   write(6,*) 'Run completed with residue ', unorm

end

! Apply a single time step
subroutine poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)

   implicit none

   integer, intent(in) :: GRIDSIZE
   real, intent(inout), dimension (0:(GRIDSIZE+1)) :: u, unew
   real, intent(in), dimension (0:(GRIDSIZE+1)) :: rho
   real, intent(in) :: hsq
   double precision, intent(out) :: unorm
   integer i, j
   
   ! Calculate one timestep
   do i = 1, GRIDSIZE
      unew(i) = 0.25*(u(i-1)+u(i+1)- hsq*rho(i))
   enddo

   ! Find the difference compared to the previous time step
   unorm = 0.0
   do i = 1, GRIDSIZE
      unorm = unorm + (unew(i)-u(i))*(unew(i)-u(i))
   enddo

   ! Overwrtie u with the new value
   do i = 1, GRIDSIZE
      u(i) = unew(i)
   enddo
end