c     a serial code for Poisson equation 

c     contact seyong.kim81@gmail.com for comments and questions 

      
      program poisson

      parameter(MAX=100,IMAX=10000)

      real u(0:(MAX+1),0:(MAX+1)), unew(0:(MAX+1),0:(MAX+1)), 
     #     rho(0:(MAX+1),0:(MAX+1))

      double precision unorm, resid

      write(6,*) 'step size = '
      read(5,*) h
      write(6,*) 'residual = '
      read(5,*) resid

      hsq = h*h

      do j = 0, MAX+1
         do i = 0, MAX+1
            u(i,j) = 0.0
            rho(i,j) = 0.0
         enddo
      enddo

      do j = 0, MAX+1
         u(0,j) = 10.0
      enddo

      do iter = 1, IMAX

         do j = 1, MAX
            do i = 1, MAX
               unew(i,j) = 0.25*(u(i-1,j)+u(i+1,j)+u(i,j-1)+u(i,j+1)
     #              - hsq*rho(i,j))
            enddo
         enddo

         unorm = 0.0
         do j = 1, MAX
            do i = 1, MAX
               unorm = unorm + (unew(i,j)-u(i,j))
     #              *(unew(i,j)-u(i,j))
            enddo
         enddo

         write(6,*) 'unorm = ', unorm

         do j = 1, MAX
            do i = 1, MAX
               u(i,j) = unew(i,j)
            enddo
         enddo

         if(sqrt(unorm) .le. sqrt(resid)) then 
            goto 1000
         endif

      enddo

 1000 continue

      do i = 0, MAX+1
         write(12,*) u(i,MAX/2)
      enddo

      do i = 0, MAX+1
         write(14,*) u(10,i)
      enddo

      write(10,*) ((u(i,j),i=0,MAX+1),j=0,MAX+1)

      stop
      end
