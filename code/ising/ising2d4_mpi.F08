! parallel code for ising model
! contact seyong.kim81@gmail.com for comments and questions

      
program ising2d

   ! 2d Ising model using Metropolis update algorithm
   ! periodic boundary condition for x- and y-direction
   ! version 3 : checker board partitioned
   ! MPI version
   ! assumes that 1-D ring topology in the y direction and no of ranks is
   ! a divisor of n2 and n2/n_ranks is an even integer for convenience

   implicit none
   include 'mpif.h'

   integer, parameter :: n1=320,n2=320
   integer, parameter :: VOLUME=n1*n2,VOLUMEd2=VOLUME/2
   integer, parameter :: n1d2=n1/2
   integer i,j,ii,ij,im,ip,is,i2,jdown,jup,ieo,ioe,n
   character(len=5) parameter_name

   integer iup(VOLUME,2),idn(VOLUME,2)

   real s(VOLUME),stmp
   real deltae, energy_now, new_energy, neighbours

   double precision esumt,magt,magsub,esumsub,esum,mag
   real random
   real beta
   integer iter
   
   integer seed_temp, seed_size
   integer, allocatable :: seed(:)
   
   integer istatus(MPI_STATUS_SIZE)
   integer itag, ierr, ireq
   real recvbuf(n1d2),sendbuf(n1d2)
   integer n_ranks, rank, nextup, nextdn
   integer n2sub, VOLUMEsub, VOLUMEd2sub


   ! Initialize MPI and set rank parameters
   call MPI_Init(ierr)
   call MPI_Comm_rank(MPI_COMM_WORLD,rank,ierr)
   call MPI_Comm_size(MPI_COMM_WORLD,n_ranks,ierr)

   n2sub = n2/n_ranks
   VOLUMEsub = n1*n2sub
   VOLUMEd2sub = VOLUMEsub/2

   if (rank .eq. (n_ranks-1)) then
      nextup = 0
   else
      nextup = rank + 1
   endif

   if (rank .eq. 0) then
      nextdn = n_ranks-1
   else
      nextdn = rank - 1
   endif

   ! Read parameters. Beta is the inverse of the temperature
   if (rank .eq. 0) then 
      open(unit=11, file="parameter")
      read(11,*) parameter_name, beta
      read(11,*) parameter_name, iter
      close(11)
   endif

   ! Broadcast parameters to all ranks
   call MPI_Bcast( beta, 1, MPI_FLOAT, 0, MPI_COMM_WORLD, ierr)
   call MPI_Bcast( iter, 1, MPI_INT, 0, MPI_COMM_WORLD, ierr)


   ! Set seed for random number generator
   call RANDOM_SEED(seed_size)
   seed_temp = 1225 + rank*seed_size
   allocate(seed(seed_size))
   do i = 1, seed_size
      seed(i) = seed_temp + i
   enddo
   call RANDOM_SEED(PUT=seed)

   do i = 1, VOLUMEsub
      call RANDOM_NUMBER(random)
      if(random .lt. 0.5) then
         s(i) = 1.0
      else
         s(i) = -1.0
      endif
   enddo

   ! Create and index of neighbours
   ! The sites are partitioned to even an odd,
   ! with even sites first in the array
   do j = 1, n2sub
      jup = j+1
      if(j .eq. n2sub) jup = 1
      jdown = j-1
      if(j .eq. 1) jdown = n2sub
      do i2 = 1, n1
         i = (i2+1)/2
         ip = (i2+2)/2
         if(i2 .eq. n1) ip = 1
         im = i2/2
         if(i2 .eq. 1) im = n1d2
         ij = i2 + j
         if (ij .eq. 2*(ij/2)) then
            is = i+n1d2*(j-1)
            iup(is,1) = ip + n1d2*(j-1) + VOLUMEd2sub
            iup(is,2) = i + n1d2*(jup-1) + VOLUMEd2sub
            idn(is,1) = im + n1d2*(j-1) + VOLUMEd2sub
            idn(is,2) = i + n1d2*(jdown-1) + VOLUMEd2sub
         else
            is = i+n1d2*(j-1)+VOLUMEd2sub
            iup(is,1) = ip + n1d2*(j-1)
            iup(is,2) = i + n1d2*(jup-1)
            idn(is,1) = im + n1d2*(j-1)
            idn(is,2) = i + n1d2*(jdown-1)
         endif
      enddo
   enddo

   ! Initialize the measurements
   esumt = 0.0
   magt = 0.0

   ! Run a number of iterations
   do n = 1, iter
      esum = 0.0
      mag = 0.0
      magsub = 0.0
      esumsub = 0.0

      ! Do for even and odd sites
      do ieo = 0, 1
         ioe = 1 - ieo
            
         ! Communicate and update the j=1 boundary
         itag = 11
         ii = ieo*VOLUMEd2sub
         do i = 1, n1d2
            sendbuf(i) = s(i+VOLUMEd2sub-n1d2+ioe*VOLUMEd2sub)
         enddo
         call MPI_Irecv(recvbuf,n1d2,MPI_REAL,nextdn,itag,MPI_COMM_WORLD,ireq,ierr)
         call MPI_Send(sendbuf,n1d2,MPI_REAL,nextup,itag,MPI_COMM_WORLD,ierr)
         call MPI_WAIT(ireq,istatus,ierr)

         do i = 1, n1d2
            neighbours = s(iup(i+ii,1))+s(iup(i+ii,2))+s(idn(i+ii,1))+recvbuf(i)
            stmp = -s(i+ii)

            energy_now = -s(i+ii)*neighbours
            new_energy = -stmp*neighbours
            deltae = new_energy-energy_now

            call RANDOM_NUMBER(random)
            if(exp(-beta*deltae) .gt. random) then
               s(i+ii) = stmp
               energy_now = new_energy
            endif

            magsub = magsub + s(i+ii)
            esumsub = esumsub + energy_now
         enddo

         ! Update the bulk of the lattice, everything but the boundaries
         do j = 2, n2sub-1
            do i = 1, n1d2
               is = i + n1d2*(j-1) + ii
               neighbours = s(iup(is,1))+s(iup(is,2))+s(idn(is,1))+s(idn(is,2))
               stmp = -s(is)
               
               energy_now = -s(is)*neighbours
               new_energy = -stmp*neighbours
               deltae = new_energy-energy_now

               call RANDOM_NUMBER(random)
               if(exp(-beta*deltae) .gt. random) then
                     s(is) = stmp
                  energy_now = new_energy
               endif
               magsub = magsub + s(is)
               esumsub = esumsub + energy_now
            enddo
         enddo
            
         ! Update the j = n2sub boundary
         itag = 22
         do i = 1, n1d2
            sendbuf(i) = s(i+ioe*VOLUMEd2sub)
         enddo
         call MPI_Irecv(recvbuf,n1d2,MPI_REAL,nextup,itag,MPI_COMM_WORLD,ireq,ierr)
         call MPI_Send(sendbuf,n1d2,MPI_REAL,nextdn,itag,MPI_COMM_WORLD,ierr)
         call MPI_WAIT(ireq,istatus,ierr)

         ii = ii+VOLUMEd2sub-n1d2
         do i = 1, n1d2
            neighbours = s(iup(i+ii,1))+recvbuf(i)+s(idn(i+ii,1))+s(idn(i+ii,2))
            stmp = -s(i+ii)
            
            energy_now = -s(i+ii)*neighbours
            new_energy = -stmp*neighbours
            deltae = new_energy-energy_now

            call RANDOM_NUMBER(random)
            if(exp(-beta*deltae) .gt. random) then
                  s(i+ii) = stmp
               energy_now = new_energy
            endif

            magsub = magsub + s(i+ii)
            esumsub = esumsub + energy_now
         enddo
         
         ! Sum the energy and magnetisation over the ranks
         call MPI_Reduce(magsub,mag,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD,ierr)
         call MPI_Reduce(esumsub,esum,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD,ierr)
      enddo

      ! Calculate average measurements and print
      if (rank .eq. 0) then 
         mag = mag/VOLUME
         esum = esum/VOLUME
         write(6,*) 'average energy = ', esum,' average magnetization = ', mag
         esumt = esumt + esum
         magt = magt + abs(mag)
      endif

   enddo

   esumt = esumt/iter
   magt = magt/iter
      
   if (rank .eq. 0) then 
      write(6,*) 'Over the whole simulation:'
      write(6,*) 'average energy = ',esumt,' average magnetization = ',magt
   endif

   call MPI_Finalize(ierr)

   stop
end
      


