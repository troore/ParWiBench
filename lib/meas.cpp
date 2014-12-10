
#include "meas.h"

double dtime()
{
	double tmseconds = 0.0;
	struct timeval mytime;

	gettimeofday(&mytime, (struct timezone*)0);
	tmseconds = (double)(mytime.tv_sec + mytime.tv_usec* 1.0e-3);

	return tmseconds;
}
