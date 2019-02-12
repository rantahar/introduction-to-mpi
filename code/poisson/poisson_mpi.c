/* a parallel code for Poisson equation */

/* contact seyong.kim81@gmail.com for comments and questions */


#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MAX 1000
#define SUBMAX 500
#define IMAX 1000

main(int argc, char** argv) {

  int i, j, flag, node, numtask, nextdn, nextup, isub, itag1, itag2, loop;
  float u[MAX+2][SUBMAX+2], unew[MAX+2][SUBMAX+2], rho[MAX+2][SUBMAX+2];
  float sendbuf[MAX],recvbuf[MAX], h, hsq;
  double unorm, usum, resid;

  MPI_Status istatus;

  FILE *fopen(), *fp12, *fp14, *fp10;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&node);
  MPI_Comm_size(MPI_COMM_WORLD,&numtask);

  if(node != (numtask-1)) nextup = node+1;
  if(node != 0) nextdn = node-1;

  h = 0.01;
  resid = 0.01;
  loop = 0;

  /*  printf("step size = \n"); scanf("%f",&h);
      printf("step size = %f\n",h);*/

  /*  printf("residual = \n"); scanf("%lf",&resid);
      printf("resid = %.7f\n",resid);*/
  
  fp12 = fopen("c.12","w");
  fp14 = fopen("c.14","w");
  fp10 = fopen("c.10","w");

  hsq = h*h;

  isub = MAX/numtask;

  for(j = 0;j <= isub+1;j++)
    for(i = 0;i <= MAX+1;i++) {
      u[i][j] = 0.0;
      rho[i][j] = 0.0;
    }
  for(j = 0;j <= isub+1;j++) u[0][j] = 10.0;

  flag = 1;

  itag1 = 22;

  itag2 = 33;

  while(flag) {
    loop++;
    for(j = 1;j <= isub;j++)
      for(i = 1;i <= MAX;i++)
	unew[i][j] = 0.25*(u[i-1][j]+u[i+1][j]+u[i][j-1]+u[i][j+1]
			   - hsq*rho[i][j]);

    unorm = 0.0;
    for(j = 1;j <= isub;j++)
      for(i = 1;i <= MAX;i++)
	unorm += (unew[i][j]-u[i][j])*(unew[i][j]-u[i][j]);

    MPI_Allreduce(&unorm,&usum,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

    if (node == 0) printf("loop = %d, unorm = %.8e\n",loop,usum);

    if (sqrt(usum) <= sqrt(resid)) flag = 0;

    for(j = 1;j <= isub;j++)
      for(i = 1;i <= MAX;i++) u[i][j] = unew[i][j];
    
    if ((node%2) == 1) {
      for(i=0;i < MAX;i++) sendbuf[i] = unew[i+1][1];
      MPI_Send(sendbuf,MAX,MPI_FLOAT,nextdn,itag1,MPI_COMM_WORLD);
      MPI_Recv(recvbuf,MAX,MPI_FLOAT,nextdn,itag2,MPI_COMM_WORLD,&istatus);
      for(i=0;i < MAX;i++) u[i+1][0] = recvbuf[i];
      if ( node != (numtask-1)) {
	for(i=0;i < MAX;i++) sendbuf[i] = unew[i+1][isub];
	MPI_Send(sendbuf,MAX,MPI_FLOAT,nextup,itag1,MPI_COMM_WORLD);
	MPI_Recv(recvbuf,MAX,MPI_FLOAT,nextup,itag2,MPI_COMM_WORLD,&istatus);
	for(i=0;i < MAX;i++) u[i+1][isub+1] = recvbuf[i];
      }
    }
    else {
      if (node != 0) {
	MPI_Recv(recvbuf,MAX,MPI_FLOAT,nextdn,itag1,MPI_COMM_WORLD,&istatus);
	for(i=0;i < MAX;i++) u[i+1][0] = recvbuf[i];
	for(i=0;i < MAX;i++) sendbuf[i] = unew[i+1][1];
	MPI_Send(sendbuf,MAX,MPI_FLOAT,nextdn,itag2,MPI_COMM_WORLD);
      }

      if (node != (numtask-1)) {
	MPI_Recv(recvbuf,MAX,MPI_FLOAT,nextup,itag1,MPI_COMM_WORLD,&istatus);
	for(i=0;i < MAX;i++) u[i+1][isub+1] = recvbuf[i];
	for(i=0;i < MAX;i++) sendbuf[i] = unew[i+1][isub];
	MPI_Send(sendbuf,MAX,MPI_FLOAT,nextup,itag2,MPI_COMM_WORLD);
      }
    }
  } 

  for(i = 0;i <= isub+1;i++) fprintf(fp12,"%f\n",u[i][MAX/2+1]);
  for(i = 0;i <= isub+1;i++) fprintf(fp14,"%f\n",u[10+1][i]);

  for(j = 0;j <= isub+1;j++)
    for(i = 0;i <= MAX+1;i++) fprintf(fp10,"%f\n",u[i][j]);
  
  MPI_Finalize();
}
