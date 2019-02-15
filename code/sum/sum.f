c     a serial code for summing, not parallelizable 

c     contact seyong.skim81@gmail.com for comments and questions 

      program test

      parameter(N=1000000)

      real A(N)
      double precision sum

      do i = 1, N
         A(i) = float(i)
      enddo

      sum = 0.0

      do i = 1, N
         sum = sum + A(i)
      enddo

      write(6,*) 'total sum = ', sum

      stop
      end
