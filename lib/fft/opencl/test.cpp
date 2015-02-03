
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fft.h"
#include "util.h"
#include "timer/meas.h"
#include "check/check.h"

#define N (1 << 12)
#define PI	3.14159265358979323846264338327950288

float v[N][2], vout[N][2], voutout[N][2], v1[N][2], v1out[N][2];

int main(int argc, char *argv[])
{
	int k;
	int n = atoi(argv[1]);
	int num_errs = 0;
	
	num_threads = atoi(argv[2]);

	// Fill v[] with a function of known FFT:
	for (k = 0; k < n; k++)
	{
		v[k][0] = 0.125 * cos((2 * PI * k) / (float)n);
		v[k][1] = 0.125 * sin((2 * PI * k ) / (float)n);
		
	//	v1[k][0] =  0.3*cos(2*PI*k/(float)N);
	//	v1[k][1] = -0.3*sin(2*PI*k/(float)N);
		v1[k][0] = v[k][0];
		v1[k][1] = v[k][1];
	}

//	print_vector("Orig", v, n);

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();

	int n_iters = 1;
	for (int i = 0; i < n_iters; i++)
	{
		fft(n, v, vout, -1);
		for (k = 0; k < n; k++)
		{
			//	v[k] = v1[k];
			//	v[k + n] = v1[k + n];
			v[k][0] = v1[k][0];
			v[k][1] = v1[k][1];
		}
	}

	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_iters * gflop_counter(n);
//	gflops = (n_gflops * 10e3) / ttime;
//	printf("%fms\n", ttime);
	printf("Number of gflops = %lf\n", n_gflops);
//	printf("GFlops = %f\n", gflops);

	for (k = 0; k < n; k++)
	{
		//	vout[k] /= n;
		//	vout[k + n] /= n;
		vout[k][0] /= n;
		vout[k][1] /= n;
	}
//	print_vector("iFFT", vout, n);
	fft(n, vout, voutout, 1);

//	print_vector(" FFT", voutout, n);

	num_errs = check_vector(v1, voutout, n);
	printf("%d\n", num_errs);
	
//	free(v);
//	free(vout);
//	free(voutout);
//	free(v1);
//	free(v1out);

	return 0;
}
