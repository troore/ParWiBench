
#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

#include "clutil.h"

#define MAX_PLATFORMS_NUM	100
#define MAX_DEVICES_NUM		100
#define MAX_BUF_SIZE		10240

cl_platform_id device_query()
{
	cl_platform_id platforms[MAX_PLATFORMS_NUM];
	cl_device_id devices[MAX_DEVICES_NUM];

	cl_uint platforms_n = 0;
	cl_uint devices_n = 0;
	cl_int _err;

	unsigned int i, j;

	/**
	 * Set platform/device/context
	 */
	CL_CHECK(clGetPlatformIDs(MAX_PLATFORMS_NUM, platforms, &platforms_n));
	printf("=== %d OpenCL platform(s) found: ===\n", platforms_n);

	if (0 == platforms_n)
	{
		printf("There is no OpenCL platform found.\n");
		exit(1);
	}

	for (i = 0; i < platforms_n; i++)
	{
		char buffer[MAX_BUF_SIZE];

		printf("  -- %d --\n", i);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, MAX_BUF_SIZE, buffer, NULL));
		printf("  Platform Profile = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, MAX_BUF_SIZE, buffer, NULL));
		printf("  Platform Version = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, MAX_BUF_SIZE, buffer, NULL));
		printf("  Platform Name = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, MAX_BUF_SIZE, buffer, NULL));
		printf("  Platform Extensions = %s\n", buffer);

		printf("\n");

//		CL_CHECK(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n));
		cl_int dev_info = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n);
		if (dev_info != CL_SUCCESS && dev_info != CL_DEVICE_NOT_FOUND)
		{
			printf("Device finding error.\n");
			exit(1);
		}
		printf("=== %d OpenCL device(s) found on platform:\n", devices_n);
		for (j = 0; j < devices_n; j++)
		{
			char buffer[MAX_BUF_SIZE];
			cl_uint buf_uint;
			cl_ulong buf_ulong;

			printf("  -- %d --\n", j);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL));
			printf("  Device Name = %s\n", buffer);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL));
			printf("  Device Vendor = %s\n", buffer);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL));
			printf("  Device Version = %s\n", buffer);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL));
			printf("  Device Max Compute Units = %u\n", buf_uint);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL));
			printf("  Device Max Clock Frequency = %u\n", buf_uint);
			CL_CHECK(clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL));
			printf("  Device Global Memory Size = %llu\n", buf_ulong);
		}
	}

#ifdef NV
	return platforms[2];
#else
	return platforms[0];
#endif
}

//void cl_params_init(const char *program_file_name, const char *kernel_file_name)
//{
//	cl_int _err;

	/* Create a device and context */
//	g_device = create_device();
//	g_context = clCreateContext(NULL, 1, &g_device, NULL, NULL, &_err);

	/* Build the program */
//	g_program = build_program(g_context, g_device, program_file_name);

	/* Create kernel */
//	g_kernel = clCreateKernel(g_program, kernel_file_name, &_err);

	/* Create a command queue */
//	g_queue = clCreateCommandQueue(g_context, g_device, CL_QUEUE_PROFILING_ENABLE, &_err);
//}

/*
void cl_params_release()
{
	clReleaseKernel(g_kernel);
	clReleaseProgram(g_program);
	clReleaseCommandQueue(g_queue);
	clReleaseContext(g_context);
}
*/

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device(cl_platform_id *p_pltfm)
{
//	cl_platform_id platform;
	cl_device_id dev;
	int _err;

	/* Identify a platform */
	/*
	_err = clGetPlatformIDs(1, &platform, NULL);
	if (_err < 0)
	{
		perror("Couldn't identify a platform");
		exit(1);
	} 
	*/

	/* Access a device */
	_err = clGetDeviceIDs(*p_pltfm, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
	if (_err == CL_DEVICE_NOT_FOUND) 
	{
		_err = clGetDeviceIDs(*p_pltfm, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}
	if (_err < 0) 
	{
		perror("Couldn't access any devices");
		exit(1);   
	}

	return dev;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context *p_ctx, cl_device_id *p_dev, const char *filename)
{
	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	int err;

	/* Read program file and place content into buffer */
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) 
	{
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
	program = clCreateProgramWithSource(*p_ctx, 1, 
			(const char**)&program_buffer, &program_size, &err);
	if(err < 0)
	{
		perror("Couldn't create the program");
		exit(1);
	}
	free(program_buffer);

	/* Build program */
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(err < 0)
	{
		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, *p_dev, CL_PROGRAM_BUILD_LOG, 
				0, NULL, &log_size);
		program_log = (char*) malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, *p_dev, CL_PROGRAM_BUILD_LOG, 
				log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);

		exit(1);
	}

	return program;
}

