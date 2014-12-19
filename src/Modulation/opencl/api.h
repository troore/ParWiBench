
#include <stdio.h>
#include <stdlib.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

cl_device_id create_device();

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

void initcl_context(cl_device_id *device,cl_context *context,cl_program *program,const char* file_name);

void initcl_kernel(cl_device_id *device, cl_context *context, cl_command_queue *queue, cl_kernel *kernel,cl_program *program, const char* kernel_name);
