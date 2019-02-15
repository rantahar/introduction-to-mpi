c     a parallel code for summing 

c     contact seyong.skim81@gmail.com for comments and questions

      
      program test_reduce

      include 'mpif.h'

      parameter (N=1000000)

      real A(N)
      double precision sum, sumt

      call MPI_INIT(ierr)
      call MPI_COMM_RANK(MPI_COMM_WORLD,node,ierr)
      call MPI_COMM_SIZE(MPI_COMM_WORLD,numtask,ierr)

      isubloop = N/numtask

      do i = 1, isubloop
         A(i) = real(i +  node*isubloop)
      enddo 

      sum = 0.0
      sumt = 0.0
      iroot = 0

      do i = 1, isubloop
         sum = sum + A(i)
      enddo 

      call MPI_REDUCE(sum,sumt,1,MPI_DOUBLE_PRECISION,MPI_SUM,iroot,
     &     MPI_COMM_WORLD,ierr)

      write(6,*) 'total sum = ', sumt

      call MPI_FINALIZE(ierr)

      stop
      end
