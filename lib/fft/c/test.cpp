
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fft.h"
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

	float *v, *vout, *voutout, *v1, *v1out;

	num_threads = atoi(argv[2]);

	v = (float *)malloc(2 * n * sizeof(float));
	vout = (float *)malloc(2 * n * sizeof(float));
	voutout = (float *)malloc(2 * n * sizeof(float));
	v1 = (float *)malloc(2 * n * sizeof(float));
	v1out = (float *)malloc(2 * n * sizeof(float));

	// Fill v[] with a function of known FFT:
	for (k = 0; k < n; k++)
	{
		v[k] = 0.125 * cos((2 * PI * k) / (float)n);
		v[k + n] = 0.125 * sin((2 * PI * k ) / (float)n);
		
	//	v1[k][0] =  0.3*cos(2*PI*k/(float)N);
	//	v1[k][1] = -0.3*sin(2*PI*k/(float)N);
		v1[k] = v[k];
		v1[k + n] = v[k + n];
	}

//	print_vector("Orig", v, n);

#ifdef _RAPL
	rapl_power_start();
#else

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_iters = 1000;
	for (int i = 0; i < n_iters; i++)
	{
		fft_nrvs(n, v, vout, -1);

		for (k = 0; k < n; k++)
		{
			v[k] = v1[k];
			v[k + n] = v1[k + n];
		}
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
		vout[k] /= n;
		vout[k + n] /= n;
	}
//	print_vector("iFFT", vout, n);
	fft_nrvs(n, vout, voutout, 1);

//	print_vector(" FFT", voutout, n);

	num_errs = check_vector(v1, voutout, n);
	printf("%d\n", num_errs);
	
	free(v);
	free(vout);
	free(voutout);
	free(v1);
	free(v1out);

	return 0;
}
