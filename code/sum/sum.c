/* a serial code for summing, not parallelizable */

/* contact seyong.skim81@gmail.com for comments and questions */

#include <stdio.h>

#define N 1000000

main(int argc,char** argv) {

int   i;
float A[N];
double sum;

for (i = 0;i < N;i++) A[i] = (float)(i+1);

sum = 0.0;

for (i = 0;i < N;i++) sum = sum + A[i];

printf("total sum = %f\n", sum);

}
