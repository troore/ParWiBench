#ifndef __CLUTIL_H_
#define __CLUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

void device_query();
void cl_params_init(const char *program_file_name, const char *kernel_file_name);
void cl_params_release();
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

extern cl_device_id g_device;
extern cl_context g_context;
extern cl_command_queue g_queue;
extern cl_program g_program;
extern cl_kernel g_kernel;

#endif
