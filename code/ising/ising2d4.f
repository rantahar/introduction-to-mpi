c     serial code for ising model

c     contact seyong.kim81@gmail.com for comments and questions

      
      program ising2d

c     2d Ising model using Metropolis update algorithm

      parameter(n1=160,n2=320,ip2d2=n1*n2,ip2=2*ip2d2)
      parameter(n1x2=n1*2)

      integer iup(ip2,2),idn(ip2,2)

      real s(ip2),stmp,esum,mag

      double precision esumt,magt

      integer*2 seed(3)
      common/seeds/seed

      seed(1) = 1225
      seed(2) = 4739
      seed(3) = 9753

      write(6,*) 'beta = '
      read(5,*) beta
      write(6,*) 'iteration = '
      read(5,*) iter

      call ranset(seed)

      do i = 1, ip2
         if(ranf() .lt. 0.5) then
            s(i) = 1.0
         else
            s(i) = -1.0
         endif
      enddo

      do j = 1, n2
         jp = j+1
         if(j .eq. n2) jp = 1
         jm = j-1
         if(j .eq. 1) jm = n2
         do i2 = 1, n1x2
            i = (i2+1)/2
            ip = (i2+2)/2
            if(i2 .eq. n1x2) ip = 1
            im = i2/2
            if(i2 .eq. 1) im = n1
            ij = i2 + j
            if (ij .eq. 2*(ij/2)) then
               is = i+n1*(j-1)
               iup(is,1) = ip + n1*(j-1) + ip2d2
               iup(is,2) = i + n1*(jp-1) + ip2d2
               idn(is,1) = im + n1*(j-1) + ip2d2
               idn(is,2) = i + n1*(jm-1) + ip2d2
            else
               is = i+n1*(j-1)+ip2d2
               iup(is,1) = ip + n1*(j-1)
               iup(is,2) = i + n1*(jp-1)
               idn(is,1) = im + n1*(j-1)
               idn(is,2) = i + n1*(jm-1)
            endif
         enddo
      enddo

c      do i = 1, ip2
c         write(6,*) 'up-x = ',iup(i,1),' dn-x = ',idn(i,1), 
c     #             ' up-y = ',iup(i,2),' dn-y = ',idn(i,2)
c      enddo

      esumt = 0.0
      magt = 0.0

      do n = 1, iter
         esum = 0.0
         mag = 0.0
         do i = 1, ip2
            env = s(iup(i,1))+s(iup(i,2))+s(idn(i,1))+s(idn(i,2))
            energy0 = -beta*s(i)*env
            stmp = -s(i)
            energy = -beta*stmp*env
            deltae = energy0-energy
            if(exp(deltae) .gt. ranf()) then
               s(i) = stmp
               energy0 = energy
            endif
            mag = mag + s(i)
            esum = esum + energy0
         enddo
         mag = mag/ip2
         esum = esum/ip2
         write(6,*) 'avearge energy = ', esum, 
     #        ' average magnetization = ', mag
         esumt = esumt + esum
         magt = magt + abs(mag)
      enddo

      esumt = esumt/iter
      magt = magt/iter
      
      write(6,*) 'over whole iterations'
      write(6,*) 'average energy = ',esumt,' average magnetiztion = ',
     #     magt

      stop
      end
      
