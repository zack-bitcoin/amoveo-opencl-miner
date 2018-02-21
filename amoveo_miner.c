#include <stdio.h>
#include <stdlib.h>
//#include <time.h>//for testing only

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int read_input(unsigned char B[32], unsigned char N[32]) {
  FILE *fileptr;
  fileptr = fopen("mining_input", "rb");
  fseek(fileptr, 0, SEEK_END);  // Jump to the end of the file
  int filelen = ftell(fileptr); // Get the current byte offset in the file
  //ftell returns a long, maybe we shouldn't truncate it.
  rewind(fileptr); 
  fread(B, 32, 1, fileptr);
  fread(N, 32, 1, fileptr);
  //N[28] = id % 256;
  //N[29] = (id / 256) % 256;
  //N[30] = ((id / 256) / 256) % 256;
  //N[31] = (((id / 256) / 256) / 256) % 256;
  unsigned char buffer[10] = { 0 };
  fread(buffer, filelen-64, 1, fileptr);
  unsigned int diff = 0;
  unsigned char c = 1;
  for (int i = 0; i < 10; i++) {
    c = buffer[i];
    if (c == 0) {
      break;
    }
    diff *= 10;
    diff += (c - '0');
  }
  fclose(fileptr); // Close the file
  return diff;
}
void write_nonce(unsigned char x[32]) {
  FILE *f = fopen("nonce.txt", "w");
  if (f == NULL) {
      printf("Error opening file!\n");
      //exit(1);
    }
  rewind(f);//unnecessary line?
  fwrite(x, 1, 32, f);
  fclose(f);
  return;
}

int main(void) {
  //create the input vector
  //66 = 32 byts of bhash + 32 bytes of nonce + 2 bytes of difficulty
  unsigned int i;
  unsigned char bhash[32];
  unsigned char nonce[32];
  /*
  char bhash[32] = {
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5,
    5,5,5,5,5,5,5,5
  };
  //char diff[2] = { 1, 200 };
  char nonce[32] = {
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4
  };
  int difficulty = 300;
  */
  unsigned int difficulty = read_input(bhash, nonce);
  printf("difficulty %u\n", difficulty);
  const int INPUT_SIZE = 1024;
  unsigned char *A = (unsigned char*)malloc(sizeof(char)*INPUT_SIZE);//maybe we should make a different nonce for each kernel.
  for(i = 0; i < 32; i++) {
    A[i] = bhash[i];
    A[i+32] = nonce[i];
    A[i+68] = 0;
  }
  A[64] = difficulty / 256;
  A[65] = difficulty % 256;
  A[66] = 0;
  A[67] = 0;
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
    //size_t global_item_size = 1000000; // How many times we run the kernel in total
    //size_t global_item_size = 5000000; // How many times we run the kernel in total
    size_t global_item_size = 5000; // How many times we run the kernel in total
    size_t local_item_size = 1; // Process in groups of 1
    //clock_t begin = clock();//for testing only.
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    unsigned char *C = (unsigned char*)malloc(sizeof(char)*INPUT_SIZE);
    ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, 
            INPUT_SIZE * sizeof(char), C, 0, NULL, NULL);
    //clock_t end = clock();//for testing only.
    //double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;//for testing only
    //double speed = 1 / time_spent;//for testing only
    //printf("speed result: %f megahashes per second per CPU\n", speed);//for testing only

    // Display the result to the screen
    for(i = 0; i < 100; i++) {
      //<<202,151,129,18,202,27,189,202,250,194,49,179,154,35,220,
      if ((i % 4) == 0) {
	printf("\n");
      }
      printf("%u ", C[i]);
    }
    unsigned char work[32];
    for(i = 0; i < 32; i++) {
      work[i] = C[i + 68];
    }
    if ((work[0] == 0) && (work[1] == 0) && (work[2] == 0) && (work[3] == 0)) {
      printf("no block found this time\n");
    } else {
      printf("found a block %u\n", work[0]);
      write_nonce(work);
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
