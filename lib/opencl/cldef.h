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

#define MAX_PLATFORMS_NUM	100
#define MAX_DEVICES_NUM	100
#define MAX_BUF_SIZE		10240
