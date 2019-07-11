! a serial code for Poisson equation 
! contact seyong.kim81@gmail.com for comments and questions 
      
program poisson

   implicit none

   integer, parameter :: MAX=100,IMAX=10000
   integer i,j

   real u(0:(MAX+1),0:(MAX+1)), unew(0:(MAX+1),0:(MAX+1))
   real rho(0:(MAX+1),0:(MAX+1))
   real h, hsq
   double precision unorm, residual

   write(6,*) 'step size = '
   read(5,*) h
   write(6,*) 'step size = ', h
   write(6,*) 'residual = '
   read(5,*) residual
   write(6,*) 'residual = ', residual

   ! Run setup
   hsq = h*h

   ! Initialise the u and rho field to 0 
   do j = 0, MAX+1
      do i = 0, MAX+1
         u(i,j) = 0.0
         rho(i,j) = 0.0
      enddo
   enddo

   ! Create a start configuration with the field
   ! u=10 at x=0
   do j = 0, MAX+1
      u(0,j) = 10.0
   enddo

   ! Run iterations until the field reaches an equilibrium
   do 

      ! Calculate one timestep
      do j = 1, MAX
         do i = 1, MAX
            unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1)- hsq*rho(i,j))
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

      if ( sqrt(unorm) <= sqrt(residual) ) then
        exit
      end if

   enddo

   ! The run is complete. Write the fields into the result files.
   ! Write the field in the middle in the y direction
   do i = 0, MAX+1
      write(12,*) u(i,MAX/2)
   enddo

   ! Write the field at x=10
   do i = 0, MAX+1
      write(14,*) u(10,i)
   enddo

   ! Write the whole u field
   write(10,*) ((u(i,j),i=0,MAX+1),j=0,MAX+1)
end
