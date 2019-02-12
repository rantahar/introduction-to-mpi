/* a serial code for Poisson equation, not parallelizable */

/* contact seyong.kim81@gmail.com for comments and questions */

#include <stdio.h>
#include <math.h>

#define MAX 100
#define IMAX 1000

main(int argc, char** argv) {

  int i, j, flag;
  float u[MAX+2][MAX+2],rho[MAX+2][MAX+2];
  float h, hsq, utmp;
  double unorm, resid;

  FILE *fopen(), *fp12, *fp14, *fp10;

  printf("step size = \n"); 
  scanf("%f",&h);
  /*  printf("step size = %f\n",h);*/

  printf("residual = \n");
  scanf("%lf",&resid);
  /*  printf("resid = %.7f\n",resid);*/
  
  fp12 = fopen("c.12","w");
  fp14 = fopen("c.14","w");
  fp10 = fopen("c.10","w");

  hsq = h*h;

  for(j = 0;j <= MAX+1;j++)
    for(i = 0;i <= MAX+1;i++) {
      u[i][j] = 0.0;
      rho[i][j] = 0.0;
    }
  for(j = 0;j <= MAX+1;j++) u[0][j] = 10.0;

  flag = 1;

  while(flag) {

    unorm = 0.0;
    for(j = 1;j <= MAX;j++)
      for(i = 1;i <= MAX;i++) {
	utmp = u[i][j];
	u[i][j] = 0.25*(u[i-1][j]+u[i+1][j]+u[i][j-1]+u[i][j+1]
			   - hsq*rho[i][j]); 
	unorm += (utmp-u[i][j])*(utmp-u[i][j]);
      }
    
    printf("unorm = %.8e\n",unorm);

    if (sqrt(unorm) <= sqrt(resid)) flag = 0;
  } 

  for(i = 0;i <= MAX+1;i++) fprintf(fp12,"%f\n",u[i][MAX/2+1]);
  for(i = 0;i <= MAX+1;i++) fprintf(fp14,"%f\n",u[10+1][i]);

  for(j = 0;j <= MAX+1;j++)
    for(i = 0;i <= MAX+1;i++) fprintf(fp10,"%f\n",u[i][j]);
}
