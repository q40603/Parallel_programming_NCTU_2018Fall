#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    long long i, num_intervals, block,left,right;
    double rect_width, area, sum, x_middle, total_sum; 

    sscanf(argv[1],"%llu",&num_intervals);
    block = num_intervals/size;
    rect_width = PI / num_intervals;

    left = block * rank;
    right = left + block;
    sum = 0;
    if (rank==0) left = 1;
    if (rank==size-1) right = num_intervals + 1;
    for(i = left; i<right; i++) {
        /* find the middle of the interval on the X-axis. */ 
        x_middle = (i - 0.5) * rect_width;
        area = sin(x_middle) * rect_width; 
        sum += area;
    } 

    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank==0) {
        if(num_intervals < size){
            total_sum = 0;
              for(i = 1; i < num_intervals + 1; i++) {

                /* find the middle of the interval on the X-axis. */ 

                x_middle = (i - 0.5) * rect_width;
                area = sin(x_middle) * rect_width; 
                total_sum = total_sum + area;
              }  
            printf("The total area is: %f\n", (float)total_sum);          
        }
        else{
            printf("The total area is: %f\n", (float)total_sum);
        }  
    }
    MPI_Finalize();
    return 0;
} 