>>program poisson
>>
>>   implicit none
>>
>>   integer, parameter :: GRIDSIZE=20
>>   integer i,j
>>
>>   real u(0:(GRIDSIZE+1),0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
>>   real rho(0:(GRIDSIZE+1),0:(GRIDSIZE+1))
>>   real h, hsq
>>   double precision unorm, difference
>>
>>   ! Set up parameters
>>   h = 0.1
>>   hsq = h*h
>>
>>   ! Initialise the u and rho field to 0 
>>   do j = 0, GRIDSIZE+1
>>      do i = 0, GRIDSIZE+1
>>         u(i,j) = 0.0
>>         rho(i,j) = 0.0
>>      enddo
>>   enddo
>>
>>   ! Test a configuration with u=10 at x=1 and y=1
>>   ! The coordinate x=1, y=1 is always in rank 0
>>   u(1,1) = 10
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
>>end