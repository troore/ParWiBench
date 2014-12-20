#ifndef __CLUTIL_H_
#define __CLUTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

#define CL_CHECK(_expr)			\
	do {						\
		cl_int _err = _expr;	\
		if (_err == CL_SUCCESS)	\
			break;				\
		fprintf(stderr, "OpenCL Error: '%s' returned %d!", #_expr, (int)_err);	\
		abort();				\
	} while (0)					\

#define CL_CHECK_ERR(_expr)				\
	({									\
	 cl_int _err = CL_INVALID_VALUE;	\
	 typeof(_expr) _ret = _expr;		\
	 if (_err != CL_SUCCESS) {			\
		fprintf(stderr, "OpenCL Error: '%s' returned %d!", #_expr, (int)_err);	\
		abort();						\
	 }									\
	 _ret;								\
	 })									\

cl_platform_id device_query();
cl_device_id create_device(cl_platform_id *p_pltfm);
cl_program build_program(cl_context *p_ctx, cl_device_id *p_dev, const char* filename);


#endif
