/* a parallel code for summing */

/* contact seyong.skim81@gmail.com for comments and questions */

#include <stdio.h>
#include <mpi.h>

#define N 1000000

main(int argc, char** argv) {

int   i, node, numtask, subN, iroot;
float A[N];
double sum, sumt;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&node);
MPI_Comm_size(MPI_COMM_WORLD,&numtask);

subN = N/numtask;

for(i = 0;i < subN;i++) A[i] = (float)(i+1 + node*subN);

sum = 0.0;
sumt = 0.0;
iroot = 0;

for(i = 0;i < subN;i++) sum = sum + A[i];

MPI_Reduce(&sum,&sumt,1,MPI_DOUBLE,MPI_SUM,iroot,MPI_COMM_WORLD);

printf("total sum = %f\n", sumt);

return MPI_Finalize();

}
