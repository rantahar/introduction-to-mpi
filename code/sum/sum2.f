c     a serial code for summing 

c     contact seyong.skim81@gmail.com for comments and questions

      program test

      parameter(N=1000000,M=100)

      real A(N)
      double precision sumt, sum(M)

      k = 0
      do i = 1, M
         do j = 1, N/M
            k = k + 1
            A(k) = float(k)
         enddo
      enddo

      sumt = 0.0
      do j = 1, N/M
         sum(j) = 0.0
      enddo

      k = 0
      do i = 1, M
         do j = 1, N/M
            k = k + 1
            sum(i) = sum(i) + A(k)
         enddo
         sumt = sumt + sum(i)
      enddo

      write(6,*) 'total sum = ', sumt

      stop
      end
