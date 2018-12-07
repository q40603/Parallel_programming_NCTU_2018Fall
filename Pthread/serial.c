#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#define SEED 35791246

int main(int argc, char** argv)
{
   unsigned long long niter=atoll(argv[1]);
   double x,y;
   unsigned long long i,count=0; /* # of points in the 1st quadrant of unit circle */
   double z;
   double pi;
   double elapsed_time;
   struct timeval t1, t2;
   gettimeofday(&t1, NULL);
   /* initialize random numbers */
   srand(SEED);
   count=0;
   for ( i=0; i<niter; i++) {
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      z = x*x+y*y;
      if (z<=1) count++;
      }
   pi=(double)count/niter*4;
   printf("pi is %g \n",pi);
    gettimeofday(&t2, NULL);
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000;
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000;
    printf("Elapsed Time: %lf seconds\n", elapsed_time / 1000);
   return 0;
}