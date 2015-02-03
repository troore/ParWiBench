
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dft.h"
#include "util.h"
#include "timer/meas.h"
#include "check/check.h"

#define PI	3.14159265358979323846264338327950288

#ifdef _RAPL
extern "C" {
#include "papi-rapl/rapl_power.h"
}
#endif

int num_threads;

int main(int argc, char *argv[])
{
	int k;
	int n = atoi(argv[1]);
	int num_errs = 0;

	float *v, *vout, *voutout;
	num_threads = atoi(argv[2]);

	v = (float *)malloc(2 * n * sizeof(float));
	vout = (float *)malloc(2 * n * sizeof(float));
	voutout = (float *)malloc(2 * n * sizeof(float));

	// Fill v[] with a function of known FFT:
	for (k = 0; k < n; k++)
	{
		v[2 * k + 0] = 0.125 * cos((2 * PI * k) / (float)n);
		v[2 * k + 1] = 0.125 * sin((2 * PI * k ) / (float)n);
	}

//	print_vector("Orig", v, n);

#ifdef _RAPL
	rapl_power_start();
#else

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_iters = 1;
	for (int i = 0; i < n_iters; i++)
	{
		dft(n, v, vout, -1);
	}

#ifndef _RAPL
	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_iters * gflop_counter(n);
	gflops = (n_gflops * 10e3) / ttime;
	printf("%fms\n", ttime);
	printf("Number of gflops = %lf\n", n_gflops);
	printf("GFlops = %f\n", gflops);
#else
	rapl_power_stop();
#endif

	for (k = 0; k < n; k++)
	{
		vout[2 * k + 0] /= n;
		vout[2 * k + 1] /= n;
	}
//	print_vector("iFFT", vout, n);
	dft(n, vout, voutout, 1);

//	print_vector(" FFT", voutout, n);

	num_errs = check_vector(v, voutout, n);
	printf("%d\n", num_errs);
	
	free(v);
	free(vout);
	free(voutout);

	return 0;
}
