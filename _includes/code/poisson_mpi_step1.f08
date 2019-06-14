>> subroutine poisson_step(u, unew, rho, GRIDSIZE, hsq, unorm)
>>
>>    implicit none
>>    include "mpif.h"
>>
>>    integer, intent(in) :: GRIDSIZE
>>    real, intent(inout), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: u, unew
>>    real, intent(in), dimension (0:(GRIDSIZE+1),0:(GRIDSIZE+1)) :: rho
>>    real, intent(in) :: hsq
>>    double precision local_unorm
>>    double precision, intent(out) :: unorm
>>    integer my_j_max, n_ranks
>>    integer ierr, i, j
>>    
>>    ! Find the number of x-slices calculated by each rank
>>    ! The simple calculation here assumes that GRIDSIZE is divisible by n_ranks
>>    call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
>>    my_j_max = GRIDSIZE/n_ranks
>> 
>>    ! Calculate one timestep
>>    do j = 1, my_j_max
>>       do i = 1, GRIDSIZE
>>          unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1) - hsq*rho(i,j))
>>       enddo
>>    enddo
>> 
>>    ! Find the difference compared to the previous time step
>>    local_unorm = 0.0
>>    do j = 1, my_j_max
>>       do i = 1, GRIDSIZE
>>          local_unorm = local_unorm + (unew(i,j)-u(i,j))*(unew(i,j)-u(i,j))
>>       enddo
>>    enddo
>> 
>>    call MPI_Allreduce( local_unorm, unorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD, ierr )
>> 
>>    ! Overwrite u with the new field
>>    do j = 1, my_j_max
>>       do i = 1, GRIDSIZE
>>          u(i,j) = unew(i,j)
>>       enddo
>>    enddo
>> 
>> end subroutine poisson_step