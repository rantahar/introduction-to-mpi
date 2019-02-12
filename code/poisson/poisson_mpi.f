c     a parallel code for Poisson equation 

c     contact seyong.kim81@gmail.com for comments and questions 

      program poisson

      parameter(MAX=1000,SUBMAX=500,IMAX=10000)

      include 'mpif.h'

      integer istatus(MPI_STATUS_SIZE)

      real u(0:(MAX+1),0:(SUBMAX+1)), unew(0:(MAX+1),0:(SUBMAX+1)), 
     #     rho(0:(MAX+1),0:(SUBMAX+1))
      real recvbuf(MAX)

      double precision unorm, usum, resid

      call MPI_INIT(ierr)

      call MPI_COMM_RANK(MPI_COMM_WORLD,node,ierr)
      call MPI_COMM_SIZE(MPI_COMM_WORLD,numtask,ierr)

      if (node .ne. (numtask-1)) nextup = node + 1

      if (node .ne. 0) nextdn = node - 1

      h = 0.01
      resid = 0.01

c      write(6,*) 'step size = '
c      read(5,*) h
c      write(6,*) 'residual = '
c      read(5,*) resid

      hsq = h*h

      isub = MAX/numtask

      do j = 0, isub+1
         do i = 0, MAX+1
            u(i,j) = 0.0
            rho(i,j) = 0.0
         enddo
      enddo

      do j = 0, isub+1
         u(0,j) = 10.0
      enddo

      itag1 = 22

      itag2 = 33
         
      do iter = 1, IMAX

         do j = 1, isub
            do i = 1, MAX
               unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1)
     #              - hsq*rho(i,j))
            enddo
         enddo

         unorm = 0.0
         do j = 1, isub
            do i = 1, MAX
               unorm = unorm + (unew(i,j)-u(i,j))
     #              *(unew(i,j)-u(i,j))
            enddo
         enddo

         call MPI_ALLREDUCE(unorm,usum,1,MPI_DOUBLE_PRECISION,MPI_SUM,
     #        MPI_COMM_WORLD,ierr)

c         write(6,*) 'node = ', node, '  unorm = ', unorm, ' usum =',
c     #        usum

         if (node .eq. 0) write(6,*) 'loop count = ', iter, ' usum =',
     #        usum

         if(sqrt(usum) .le. sqrt(resid)) then 
            goto 1000
         endif

         do j = 1, isub
            do i = 1, MAX
               u(i,j) = unew(i,j)
            enddo
         enddo

         if (mod(node,2) .eq. 1) then

            call MPI_SEND(unew(1,1),MAX,MPI_REAL,nextdn,itag1,
     #           MPI_COMM_WORLD,ierr1)
            call MPI_RECV(u(1,0),MAX,MPI_REAL,nextdn,itag2,
     #           MPI_COMM_WORLD,istatus,ierr2)

            if (node .lt. (numtask-1)) then
               call MPI_SEND(unew(1,isub),MAX,MPI_REAL,nextup,itag1,
     #              MPI_COMM_WORLD,ierr1)
               call MPI_RECV(u(1,isub+1),MAX,MPI_REAL,nextup,itag2,
     #              MPI_COMM_WORLD,istatus,ierr2)
            endif
            
         else

            if (node .gt. 0) then
               call MPI_RECV(u(1,0),MAX,MPI_REAL,nextdn,itag1,
     #              MPI_COMM_WORLD,istatus,ierr1)
               call MPI_SEND(unew(1,1),MAX,MPI_REAL,nextdn,itag2,
     #           MPI_COMM_WORLD,ierr2)
            endif

            if (node .lt. (numtask-1)) then
               call MPI_RECV(u(1,isub+1),MAX,MPI_REAL,nextup,itag1,
     #              MPI_COMM_WORLD,istatus,ierr1)
               call MPI_SEND(unew(1,isub),MAX,MPI_REAL,nextup,itag2,
     #              MPI_COMM_WORLD,ierr2)
            endif

         endif

      enddo

 1000 continue

c      do i = 0, isub+1
c         write(20+node,*) u(i,MAX/2)
c      enddo

      do i = 0, isub+1
         write(40+node,*) u(10,i)
      enddo

      write(10+node,*) ((u(i,j),i=0,isub+1),j=0,isub+1)

      call MPI_FINALIZE(ierr)

      stop
      end
