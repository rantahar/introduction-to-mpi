/* a serial code for summing */

/* contact seyong.skim81@gmail.com for comments and questions */

#include <stdio.h>

#define N 1000000
#define M 100

main(int argc,char** argv) {

int   i, j, k;
float A[N];
double sumt, sum[M];

k = 0;
for (i = 0;i < M;i++) 
   for (j = 0;j < (N/M);j++) {
      k = k+1;
      A[k-1] = (float)(k);
}
sumt = 0.0;
for (j = 0;j < M;j++) sum[j] = 0.0;

k = -1;
for (i = 0;i < M;i++) {
   for (j = 0;j < (N/M);j++) {
      k = k + 1;
      sum[i] = sum[i] + A[k];
   }
   sumt = sumt + sum[i];
}

printf("total sum = %f\n", sumt);

}
