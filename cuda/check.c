#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

float data1[1000050], data2[1000050];
int size_of_data1, size_of_data2;
void read_data1(){
	FILE *fp;
	fp = fopen("output_1.txt", "r");
	fscanf(fp, "%d", &size_of_data1);
	for(int i=0; i<size_of_data1; i++) {
		fscanf(fp, "%f ", &data1[i]);
	}
	fclose(fp);
}
void read_data2(){
	FILE *fp;
	fp = fopen("output_2.txt", "r");
	fscanf(fp, "%d", &size_of_data2);
	for(int i=0; i<size_of_data2; i++) {
		fscanf(fp, "%f ", &data2[i]);
	}
	fclose(fp);
}
int main(){
	read_data1();
	read_data2();
	int count = 0;	
	for(int i = 0 ; i< size_of_data1 ; i++){
		float tmp = data1[i] - data2[i];
		if(tmp > 0.01 || tmp < -0.01){
			count++;
		}
		printf("%6.4f %6.4f %d\n",data1[i], data2[i],count);
	}
	//printf("%d\n", count);
}