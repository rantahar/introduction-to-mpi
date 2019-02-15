c     parallel code for ising model

c     contact seyong.kim81@gmail.com for comments and questions

      
      program ising2d

c     2d Ising model using Metropolis update algorithm
c     periodic boundary condition for x- and y-direction
c     version 3 : checker board partitioned
c     MPI version
c     assumes that 1-D ring topology in the y direction and no of nodes is
c     a divisor of n2 and n2/numtask is an even integer for convenience

      parameter(n1=160,n2=320,ip2d2=n1*n2,ip2=2*ip2d2)
      parameter(n1x2=n1*2)

      include '/usr/local/lam/h/mpif.h'

      integer istatus(MPI_STATUS_SIZE)

      integer iup(ip2,2),idn(ip2,2)

      real s(ip2),stmp
      real recvbuf(n1),sendbuf(n1)

      double precision esumt,magt,magsub,esumsub,esum,mag

      integer*2 seed(3)
      common/seeds/seed

      call MPI_INIT(ierr)

      call MPI_COMM_RANK(MPI_COMM_WORLD,node,ierr)
      call MPI_COMM_SIZE(MPI_COMM_WORLD,numtask,ierr)

      seed(1) = 1225 + node
      seed(2) = 4739 + node
      seed(3) = 9753 + node

      iroot = 0
      n2sub = n2/numtask
      ip2sub = n1x2*n2sub
      ip2d2sub = ip2sub/2

      read(12,*) beta, iter

      if (node .eq. (numtask-1)) then
         nextup = 0
      else
         nextup = node + 1
      endif

      if (node .eq. 0) then
         nextdn = numtask-1
      else
         nextdn = node - 1
      endif

c      write(6,*) 'node = ', node, nextup, nextdn
      
      call ranset(seed)

      do i = 1, ip2sub
         if(ranf() .lt. 0.5) then
            s(i) = 1.0
         else
            s(i) = -1.0
         endif
      enddo

      do j = 1, n2sub
         jp = j+1
         if(j .eq. n2sub) jp = 1
         jm = j-1
         if(j .eq. 1) jm = n2sub
         do i2 = 1, n1x2
            i = (i2+1)/2
            ip = (i2+2)/2
            if(i2 .eq. n1x2) ip = 1
            im = i2/2
            if(i2 .eq. 1) im = n1
            ij = i2 + j
            if (ij .eq. 2*(ij/2)) then
               is = i+n1*(j-1)
               iup(is,1) = ip + n1*(j-1) + ip2d2sub
               iup(is,2) = i + n1*(jp-1) + ip2d2sub
               idn(is,1) = im + n1*(j-1) + ip2d2sub
               idn(is,2) = i + n1*(jm-1) + ip2d2sub
            else
               is = i+n1*(j-1)+ip2d2sub
               iup(is,1) = ip + n1*(j-1)
               iup(is,2) = i + n1*(jp-1)
               idn(is,1) = im + n1*(j-1)
               idn(is,2) = i + n1*(jm-1)
            endif
         enddo
      enddo

      esumt = 0.0
      magt = 0.0

      do n = 1, iter

         esum = 0.0
         mag = 0.0
         
         magsub = 0.0
         esumsub = 0.0

         do ieo = 0, 1
            ioe = 1 - ieo
            
c     j = 1 boundary

            itag = 11
            
            ii = ieo*ip2d2sub
            do i = 1, n1
               sendbuf(i) = s(i+ip2d2sub-n1+ioe*ip2d2sub)
            enddo
            
            call MPI_IRECV(recvbuf,n1,MPI_REAL,nextdn,itag,
     #           MPI_COMM_WORLD,ireq,ierr)
            
            call MPI_SEND(sendbuf,n1,MPI_REAL,nextup,itag,
     #           MPI_COMM_WORLD,ierr)

            call MPI_WAIT(ireq,istatus,ierr)
            
            do i = 1, n1
               env = s(iup(i+ii,1))+s(iup(i+ii,2))
     #              +s(idn(i+ii,1))+recvbuf(i)
               energy0 = -beta*s(i+ii)*env
               stmp = -s(i+ii)
               energy = -beta*stmp*env
               deltae = energy0-energy
               if(exp(deltae) .gt. ranf()) then
                  s(i+ii) = stmp
                  energy0 = energy
               endif
               magsub = magsub + s(i+ii)
               esumsub = esumsub + energy0
            enddo

            do j = 2, n2sub-1
               do i = 1, n1
                  is = i + n1*(j-1) + ii
                  env = s(iup(is,1))+s(iup(is,2))
     #                 +s(idn(is,1))+s(idn(is,2))
                  energy0 = -beta*s(is)*env
                  stmp = -s(is)
                  energy = -beta*stmp*env
                  deltae = energy0-energy
                  if(exp(deltae) .gt. ranf()) then
                     s(is) = stmp
                     energy0 = energy
                  endif
                  magsub = magsub + s(is)
                  esumsub = esumsub + energy0
               enddo
            enddo
            
c     j = n2sub boundary

            itag = 22

            do i = 1, n1
               sendbuf(i) = s(i+ioe*ip2d2sub)
            enddo

            call MPI_IRECV(recvbuf,n1,MPI_REAL,nextup,itag,
     #           MPI_COMM_WORLD,ireq,ierr)
            
            call MPI_SEND(sendbuf,n1,MPI_REAL,nextdn,itag,
     #           MPI_COMM_WORLD,ierr)
            
            call MPI_WAIT(ireq,istatus,ierr)

            ii = ii+ip2d2sub-n1
            do i = 1, n1
               env = s(iup(i+ii,1))+recvbuf(i)
     #              +s(idn(i+ii,1))+s(idn(i+ii,2))
               energy0 = -beta*s(i+ii)*env
               stmp = -s(i+ii)
               energy = -beta*stmp*env
               deltae = energy0-energy
               if(exp(deltae) .gt. ranf()) then
                  s(i+ii) = stmp
                  energy0 = energy
               endif
               magsub = magsub + s(i+ii)
               esumsub = esumsub + energy0
            enddo
         
            call MPI_REDUCE(magsub,mag,1,MPI_DOUBLE_PRECISION,
     #           MPI_SUM,iroot,MPI_COMM_WORLD,ierr)
            call MPI_REDUCE(esumsub,esum,1,MPI_DOUBLE_PRECISION,
     #           MPI_SUM,iroot,MPI_COMM_WORLD,ierr)
         enddo
         if (node .eq. 0) then 
            mag = mag/ip2
            esum = esum/ip2
            write(6,*) 'avearge energy = ', esum, 
     #           ' average magnetization = ', mag
            esumt = esumt + esum
            magt = magt + abs(mag)
         endif

      enddo

      esumt = esumt/iter
      magt = magt/iter
      
      if (node .eq. 0) then 
         write(6,*) 'over whole iterations'
         write(6,*) 'average energy = ',esumt,' average magnetiztion = '
     #        ,magt
      endif

      call MPI_FINALIZE(ierr)

      stop
      end
      


