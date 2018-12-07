/**********************************************************************
 * DESCRIPTION:
 *   Serial Concurrent Wave Equation - C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
void init_line(void);
void update (void);
void printfinal (void);

int nsteps,                     /* number of time steps */
    tpoints,                /* total points along string */
    rcode;                      /* generic return code */
float  values[MAXPOINTS];     /* values at time t */

/**********************************************************************
 *  Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n", 
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Initialize points on line
 *********************************************************************/
__global__
void init(float *values, int num_of_points){
   int index = blockIdx.x*blockDim.x + threadIdx.x;
   float x;
   //fac = 2.0 * PI;
   //tmp = num_of_points;
   x = 1.0*(float)index / (float)num_of_points;
   values[index] = sin (6.2831853 * x);
}
void init_line(void)
{
   int j;
   float x, fac, k, tmp;

   /* Calculate initial values based on sine curve */
   fac = 2.0 * PI;
   k = 0.0;
   tmp = tpoints - 1;
   for (j = 0; j < tpoints; j++) {
      x = k/tmp;
      values[j] = sin (fac * x);
      k = k + 1.0;
   }
}

/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/
__global__
void do_math(float *values, int num_of_points, int num_of_iterations)
{
   int index = blockIdx.x*blockDim.x + threadIdx.x;
   if (index < num_of_points && index > 0)
   {
      float old_val, val, new_val;
      old_val = val = values[index];
      for (int i = 0; i < num_of_iterations; i++)
      {
          new_val = (1.82 * val) - old_val;
          old_val = val;
          val = new_val;
      }
      values[index] = val;
   }
}

/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/
void update()
{

   float *arr;
   int size = tpoints * sizeof(float);
   cudaMalloc((void**)&arr, size);
   cudaMemcpy(arr, values, size, cudaMemcpyHostToDevice);
   /* Update values for each time step */
   do_math<<<((tpoints + 1023) >> 10), 1024>>>(arr, tpoints, nsteps);
   cudaMemcpy(values, arr, size, cudaMemcpyDeviceToHost);
   cudaFree(arr);
   values[0] = values[tpoints-1] = 0.0;
}

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 0; i < tpoints; i++) {
      printf("%6.4f ", values[i]);
      if ((i+1)%10 == 0)
         printf("\n");
   }
}
void output_data(int n,int point){
   char filename[100];
   FILE *fp;
   sprintf(filename, "output_%d.txt", n);
   fp = fopen(filename, "w");
   fprintf(fp, "%d\n", point);
   for (int i = 0; i < tpoints; i++) {
      fprintf(fp, "%6.4f ", values[i]);
   }
   fprintf(fp, "\n");

   fclose(fp); 
}
/**********************************************************************
 * Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
   sscanf(argv[1],"%d",&tpoints);
   sscanf(argv[2],"%d",&nsteps);
   check_param();
   printf("Initializing points on the line...\n");
   init_line();
   printf("Updating all points for all time steps...\n");
   update();
   printf("Printing final results...\n");
   printfinal();
   //output_data(1,tpoints);
   printf("\nDone.\n\n");
   
   return 0;
}