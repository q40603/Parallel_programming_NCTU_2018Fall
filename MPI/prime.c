#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define FIRST     0 
int isprime(long long int n) {
  // var
  long long int i;
  long long int squareroot;

  if (n > 10) {
    if (n % 2 == 0 || n % 3 == 0) 
      return 0; 
    squareroot = sqrt(n);
    for (i = 5; i <= squareroot; i = i + 6)
       if (n % i == 0 || n % (i + 2) == 0)
        return 0;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[])
{
  long long int foundone,maxprime;
  long long int pc,pcsum;
  long long int n, limit;
  int size;
  int rank;
  long long int left;
  long long int right;
  long long int block;
  sscanf(argv[1], "%llu", &limit);
  

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == FIRST)printf("Starting. Numbers to be scanned = %lld\n", limit);
  pc = 0;

  block = (limit - 11) / size + 1;
  left = rank * block + 11 + 1;
  if(rank == size -1)right = limit;
  else{
    right = (rank+1) * block + 11;  
  }
  right -= !(right & 1);
  left += !(left & 1);
  if( rank!=FIRST ){
    for (n = left; n <= right && n < limit; n += 2) {
      if (isprime(n)) {
        pc++;
        foundone = n;
      }
    }
    MPI_Reduce(&pc,&pcsum,1,MPI_LONG_LONG_INT,MPI_SUM,FIRST,MPI_COMM_WORLD);
    MPI_Reduce(&foundone,&maxprime,1,MPI_LONG_LONG_INT,MPI_MAX,FIRST,MPI_COMM_WORLD);
  }
  else{
      if(limit <= 13){
        if(limit > 11){
          pc = 5;
          foundone = 11;
        } 
        else if(limit > 7){
          pc = 4;
          foundone = 7;
        } 
        else if(limit > 5){
          pc = 3;
          foundone = 5;
        } 
        else if(limit > 3){
          pc = 2;
          foundone = 3; 
        } 
        else if(limit > 2){
          pc = 1;
          foundone = 2;
        }    
      }
      else{
        pc = 5;
        for (n = left; n <= right && n < limit; n += 2) {
          if (isprime(n)) {
            pc++;
            foundone = n;
          }
        }        
      }

      MPI_Reduce(&pc,&pcsum,1,MPI_LONG_LONG_INT,MPI_SUM,FIRST,MPI_COMM_WORLD);
      MPI_Reduce(&foundone,&maxprime,1,MPI_LONG_LONG_INT,MPI_MAX,FIRST,MPI_COMM_WORLD);
      printf("Done. Largest prime is %lld Total primes %lld \n",maxprime,pcsum);
  }

  MPI_Finalize();
  return 0;
}