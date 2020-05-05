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
>>    integer status(MPI_STATUS_SIZE)
>>    integer n_ranks, my_rank
>>    integer ierr, i
>>    
>>    ! Find the number of x-slices calculated by each rank
>>    ! The simple calculation here assumes that GRIDSIZE is divisible by n_ranks
>>    call MPI_Comm_rank(MPI_COMM_WORLD, my_rank, ierr)
>>    call MPI_Comm_size(MPI_COMM_WORLD, n_ranks, ierr)
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
>>   ! The u field has been changed, communicate it to neighbours
>>   ! With blocking communication, half the ranks should send first
>>   ! and the other half should receive first
>>   if (mod(my_rank,2) == 1) then
>>       ! Ranks with odd number send first
>>   
>>       ! Send data down from my_rank to my_rank-1
>>       call MPI_Send( u(1), 1, MPI_REAL, my_rank-1, 1, MPI_COMM_WORLD, ierr)
>>       ! Receive dat from my_rank-1
>>       call MPI_Recv( u(0), 1, MPI_REAL, my_rank-1, 2, MPI_COMM_WORLD, status, ierr)
>>   
>>       if (my_rank < (n_ranks-1)) then
>>          call MPI_Send( u(GRIDSIZE),   1, MPI_REAL, my_rank+1, 1, MPI_COMM_WORLD, ierr)
>>          call MPI_Recv( u(GRIDSIZE+1), 1, MPI_REAL, my_rank+1, 2, MPI_COMM_WORLD, status, ierr)
>>       endif
>>   
>>   else
>>       ! Ranks with even number receive first
>>   
>>       if (my_rank > 0) then
>>          call MPI_Recv( u(0), 1, MPI_REAL, my_rank-1, 1, MPI_COMM_WORLD, status, ierr)
>>          call MPI_Send( u(1), 1, MPI_REAL, my_rank-1, 2, MPI_COMM_WORLD, ierr)
>>       endif
>>   
>>       if (my_rank < (n_ranks-1)) then
>>          call MPI_Recv( u(GRIDSIZE+1), 1, MPI_REAL, my_rank+1, 1, MPI_COMM_WORLD, status, ierr)
>>          call MPI_Send( u(GRIDSIZE),   1, MPI_REAL, my_rank+1, 2, MPI_COMM_WORLD, ierr)
>>       endif
>>   endif
>> 
>> end subroutine poisson_step