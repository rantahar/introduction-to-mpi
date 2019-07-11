! a serial code for Poisson equation 
! contact seyong.kim81@gmail.com for comments and questions 


program poisson_main

   use poisson

   implicit none

   integer, parameter :: MAX=100
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

   ! Test a configuration with u=10 at x=1 and y=1
   u[1][1] = 10;

   ! Run a single iteration of the poisson solver
   call poisson_step( u, unew, rho, MAX, hsq, unorm )

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

end program poisson_main


