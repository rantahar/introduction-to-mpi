>> subroutine poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)
>>
>>    implicit none
>>    include "mpif.h"
>>
>>    integer, intent(in) :: GRIDSIZE
>>    real, intent(inout), dimension (0:(GRIDSIZE+1)) :: u, unew
>>    real, intent(in), dimension (0:(GRIDSIZE+1)) :: rho
>>    real, intent(in) :: hsq
>>    double precision local_unorm
>>    double precision, intent(out) :: unorm
>>    integer my_j_max, n_ranks
>>    integer ierr, i
>>    
>>    ! Calculate one timestep
>>    do i = 1, GRIDSIZE
>>      unew(i) = 0.5*(u(i-1)+u(i+1)- hsq*rho(i))
>>    enddo
>> 
>>    ! Find the difference compared to the previous time step
>>    local_unorm = 0.0
>>    do i = 1, GRIDSIZE
>>        local_unorm = local_unorm + (unew(i)-u(i))*(unew(i)-u(i))
>>    enddo
>> 
>>    call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )
>> 
>>    ! Overwrite u with the new field
>>    do i = 1, GRIDSIZE
>>        u(i) = unew(i)
>>    enddo
>> 
>> end subroutine poisson_step