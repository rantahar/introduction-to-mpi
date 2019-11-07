>>program poisson
>>
>>   implicit none
>>
>>   integer, parameter :: GRIDSIZE=10
>>   integer i,j
>>
>>   real u(0:(GRIDSIZE+1)), unew(0:(GRIDSIZE+1))
>>   real rho(0:(GRIDSIZE+1))
>>   real h, hsq
>>   double precision unorm, difference
>>
>>   ! Set up parameters
>>   h = 0.1
>>   hsq = h*h
>>
>>   ! Initialise the u and rho field to 0 
>>   do i = 0, GRIDSIZE+1
>>       u(i) = 0.0
>>       rho(i) = 0.0
>>    enddo
>>   
>>
>>   ! Test a configuration with u=10 at x=0 boundary
>>   u(0) = 10
>>
>>   ! Run a single iteration of the poisson solver
>>   call poisson_step( u, unew, rho, GRIDSIZE, hsq, unorm )
>>   
>>   if (unorm == 25) then
>>      write(6,*) "PASSED after 1 step"
>>   else
>>      write(6,*) "FAILED after 1 step"
>>   end if
>>
>>   ! Run 9 more iterations for a total of 10
>>   do i = 1, 10
>>      call poisson_step( u, unew, rho, GRIDSIZE, hsq, unorm )
>>   end do
>>
>>   difference = unorm - 0.463676
>>   if (difference*difference < 1e-16) then
>>      write(6,*) "PASSED after 50 steps"
>>   else
>>      write(6,*) "FAILED after 50 steps"
>>   end if
>>
>>end