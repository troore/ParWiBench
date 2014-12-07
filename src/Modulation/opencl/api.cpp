#define _CRT_SECURE_NO_WARNINGS
#include "api.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

   cl_platform_id platform;
	cl_platform_id platforms[10];
   cl_device_id dev;
   int err;

 err = clGetPlatformIDs(10, platforms, NULL);
   if(err < 0) {
      perror("Couldn't find any platforms");
      exit(1);
   }
   platform = platforms[2];

   /* Access a device */
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

	size_t local_size;
	err = clGetDeviceInfo(dev, CL_DEVICE_MAX_WORK_GROUP_SIZE, 	
         	sizeof(local_size), &local_size, NULL);	
   	if(err < 0) {
      	perror("Couldn't obtain device information");
      	exit(1);   
   }
   return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {
      /* Find size of log and print to std output */
	clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	program_log = (char*) malloc(log_size + 1);
       	program_log[log_size] = '\0';
       	clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
		       	log_size + 1, program_log, NULL);
       	printf("%s\n", program_log);
       	free(program_log);
       	exit(1);
   }
   return program;
}
void initcl_context(cl_device_id *device,cl_context *context,cl_program *program,const char* file_name)
{
	int err;
	/* Create device and determine local size */
   	*device = create_device();
	/* Create a context */
   	*context = clCreateContext(NULL, 1, device, NULL, NULL, &err);
   	if(err < 0) {
      		perror("Couldn't create a context");
      		exit(1);   
   	}	
	/* Build program */
   	*program = build_program(*context, *device, file_name);
	//printf("%d\n",program);
}
void initcl_kernel(cl_device_id *device, 
				cl_context *context, 
				cl_command_queue *queue, 
				cl_kernel *kernel,
				cl_program *program,
				const char* kernel_name)
{
	int err;
	/* Create a command queue */
  // queue = clCreateCommandQueue(context, device, 
        // CL_QUEUE_PROFILING_ENABLE, &err);
	// /* Create kernels */
	*kernel = clCreateKernel(*program, kernel_name, &err);
       	if(err < 0) {
	       	perror("Couldn't create a kernel");
	      	exit(1);
       	}

   //printf("success\n");
   *queue = clCreateCommandQueue(*context,*device,CL_QUEUE_PROFILING_ENABLE,&err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

}






