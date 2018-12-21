#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <CL/opencl.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#pragma optimize level=3
#pragma GCC optimize("-O3")
// OpenCL kernel. Each work item takes care of one element of c
const char *kernelSource =                                       "\n" \
"#pragma OPENCL                                                  \n" \
"__kernel void rgb_count(  __global unsigned int *a,             \n" \
"                       __global unsigned int *c,                \n" \
"                       const unsigned int n)                    \n" \
"{                                                               \n" \
"    //Get our global thread ID                                  \n" \
"    int id = get_global_id(0);                                  \n" \
"    int pos = (id % 3) << 8;                                    \n" \
"    if (id < n)                                                 \n" \
"       atomic_inc(&c[pos + a[id]]);                             \n" \
"                                                                \n" \
"}                                                               \n" \
                                                                "\n" ;
int main(int argc, char const *argv[])
{
    int sec,usec;
    struct timeval start, end;
    gettimeofday(&start, 0); 


    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel

    // Device input buffers
    cl_mem d_a;
    // Device output buffer
    cl_mem d_c;


	unsigned int * histogram_results;
	unsigned int i=0, a,b,c,n;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0516225.out", std::ios_base::out);

	inFile >> n;
	unsigned int *image = new unsigned int[n];
	histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	memset (histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));
	while( inFile >> a >> b >> c ) {
		image[i++] = a;
		image[i++] = b;
		image[i++] = c;
	}  
    //printf("Total elapsed time: %.4f sec \n", sec + (usec/1000000.0)); 
	size_t bytes = sizeof(unsigned int) *n;
    cl_int err;
 
    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);
    /*if(err == CL_SUCCESS){
    	std::cout<<"clGetPlatformIDs CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clGetPlatformIDs CL_ERROR"<<std::endl;
    }*/
 
    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    /*if(err == CL_SUCCESS){
    	std::cout<<"clGetDeviceIDs CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clGetDeviceIDs CL_ERROR"<<std::endl;
    } */
    // Create a context 
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    /*if(err == CL_SUCCESS){
    	std::cout<<"clCreateContext CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clCreateContext CL_ERROR"<<std::endl;
    } */ 
    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    /*if(err == CL_SUCCESS){
    	std::cout<<"clCreateCommandQueue CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clCreateCommandQueue CL_ERROR"<<std::endl;
    }  */
    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1,
                            (const char **) & kernelSource, NULL, &err);
 
    /*if(err == CL_SUCCESS){
    	std::cout<<"clCreateProgramWithSource CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clCreateProgramWithSource CL_ERROR"<<std::endl;
    }  */
    // Build the program executable
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
 
    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "rgb_count", &err);

    /*if(err == CL_SUCCESS){
    	std::cout<<"clCreateKernel CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clCreateKernel CL_ERROR"<<std::endl;
    } */
 
    // Create the input and output arrays in device memory for our calculation
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,sizeof(unsigned int) * 256 * 3, NULL, NULL);
 
    // Write our data set into the input array in device memory
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0,bytes, image, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, d_c, CL_TRUE, 0,sizeof(unsigned int) * 256 * 3, histogram_results, 0, NULL, NULL);

    // Set the arguments to our compute kernel
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &n);

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &bytes, 0,
                                                              0, NULL, NULL);
    /*if(err == CL_SUCCESS){
    	std::cout<<"clEnqueueNDRangeKernel CL_SUCCESS"<<std::endl;
    }
    else{
    	std::cout<<"clEnqueueNDRangeKernel CL_ERROR"<<std::endl;
    } */

    // Wait for the command queue to get serviced before reading back results
    //clFinish(queue);
 
    // Read the results from the device
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0,
                                sizeof(unsigned int) * 256 * 3, histogram_results, 0, NULL, NULL );    

	//histogram_results = histogram(image, n);
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();
    // release OpenCL resources
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

 
	return 0;
}