#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
  //create the input vector
  //66 = 32 byts of bhash + 32 bytes of nonce + 2 bytes of difficulty
  int i;
  char bhash[32] = {
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5
  };
  char diff[2] = { 6, 7 };
  char nonce[32] = {
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4
  };
  const int INPUT_SIZE = 1024;
  int *A = (int*)malloc(sizeof(char)*INPUT_SIZE);//maybe we should make a different nonce for each kernel.
  for(i = 0; i < 32; i++) {
    A[i] = bhash[i];
    A[i+32] = nonce[i];
  }
  A[64] = diff[0];
  A[65] = diff[1];

  FILE *fp;
  char *source_str;
  size_t source_size;

  fp = fopen("amoveo_miner.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose( fp );

  // Get platform and device information
  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;   
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, 
			&device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector 
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, INPUT_SIZE * sizeof(char), NULL, &ret);

    // Copy the lists A and B to it's memory buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, INPUT_SIZE * sizeof(char), A, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "amoveo_mine", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = 1; // How many times we run the kernel in total
    size_t local_item_size = 1; // Process in groups of 1
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int *C = (int*)malloc(sizeof(char)*INPUT_SIZE);
    ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, 
            INPUT_SIZE * sizeof(char), C, 0, NULL, NULL);

    // Display the result to the screen
    for(i = 0; i < 10; i++) {
      //<<202,151,129,18,202,27,189,202,250,194,49,179,154,35,220,
      unsigned int x = C[i];
      int a = x % 256;
      int b = (x / 256) % 256;
      int c = ((x / 256) / 256) % 256;
      int d = (((x / 256) / 256) / 256) % 256;
      printf("%d %d %d %d\n", d, c, b, a);
    }

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    //ret = clReleaseMemObject(b_mem_obj);
    //ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    //free(B);
    free(C);
    return 0;
}
