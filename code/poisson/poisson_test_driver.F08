program fruit_driver
  use fruit
  use poisson_test
  implicit none
  include "mpif.h" 

  integer ierr
  
  ! First call MPI_Init
  call MPI_Init(ierr)

  call init_fruit

  call test_poisson_step

  call fruit_summary
  call fruit_finalize

  ! Call MPI_Finalize at the end
  call MPI_Finalize(ierr)
end program fruit_driver
