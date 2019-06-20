! serial code for ising model
! contact seyong.kim81@gmail.com for comments and questions

program ising2d

   implicit none

   ! 2d Ising model using Metropolis update algorithm

   integer, parameter :: n1=320,n2=320
   integer, parameter :: VOLUME=n1*n2,VOLUMEd2=VOLUME/2
   integer, parameter :: n1d2=n1/2
   integer i,j,ij,im,ip,is,i2,jdown,jup,n
   character(len=5) parameter_name
   
   integer iup(VOLUME,2),idn(VOLUME,2)

   real s(VOLUME),stmp,esum,mag
   real deltae, energy_now, new_energy, neighbours
   double precision esumt,magt
   real random
   real beta
   integer iter

   integer seed_temp, seed_size
   integer, allocatable :: seed(:)

   ! Read parameters. Beta is the inverse of the temperature
   open(unit=11, file="parameter")
   read(11,*) parameter_name, beta
   read(11,*) parameter_name, iter
   close(11)
     
   ! Set seed for random number generator
   seed_temp = 1225
   call RANDOM_SEED(seed_size)
   allocate(seed(seed_size))
   do i = 1, seed_size
      seed(i) = seed_temp + i
   enddo
   call RANDOM_SEED(PUT=seed)

   ! Initialize each point randomly
   do i = 1, VOLUME
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
   do j = 1, n2
      jup = j+1
      if(j .eq. n2) jup = 1
      jdown = j-1
      if(j .eq. 1) jdown = n2
      do i2 = 1, n1
         i = (i2+1)/2
         ip = (i2+2)/2
         if(i2 .eq. n1) ip = 1
         im = i2/2
         if(i2 .eq. 1) im = n1d2
         ij = i2 + j
         if (ij .eq. 2*(ij/2)) then
            is = i+n1d2*(j-1)
            iup(is,1) = ip + n1d2*(j-1) + VOLUMEd2
            iup(is,2) = i + n1d2*(jup-1) + VOLUMEd2
            idn(is,1) = im + n1d2*(j-1) + VOLUMEd2
            idn(is,2) = i + n1d2*(jdown-1) + VOLUMEd2
         else
            is = i+n1d2*(j-1)+VOLUMEd2
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

      ! Loop over the lattice and try to flip each atom
      do i = 1, VOLUME
         neighbours = s(iup(i,1))+s(iup(i,2))+s(idn(i,1))+s(idn(i,2))
         stmp = -s(i)

         ! Find the energy before and after the flip
         energy_now = s(i)*neighbours
         new_energy = stmp*neighbours
         deltae = energy_now-new_energy

         ! Accept or reject the change
         CALL RANDOM_NUMBER(random)
         if(exp(-beta*deltae) .gt. random) then
            s(i) = stmp
            energy_now = new_energy
         endif

         ! Measure magnetisation and energy
         mag = mag + s(i)
         esum = esum + energy_now
      enddo

      ! Calculate measurements and add to run averages
      mag = mag/VOLUME
      esum = esum/VOLUME
      esumt = esumt + esum
      magt = magt + abs(mag)

      write(6,*) 'average energy = ', esum, ' average magnetization = ', mag
   enddo

   esumt = esumt/iter
   magt = magt/iter
      
   write(6,*) 'over whole simulation'
   write(6,*) 'average energy = ',esumt,' average magnetization = ', magt

   stop
end
      
