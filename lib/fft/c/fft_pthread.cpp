
/* Factored discrete Fourier transform, or FFT, and its inverse iFFT */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "fft.h"
#include "util.h"

typedef struct
{
	float *a;
	float *y;
	int n;
//	int i;
	int start, end;
	int p;
	int direction;
}butterfly_parms;

void *thread_butterfly(void *p_parms_struct)
{
	float omega_m[2], omega[2];
	float t[2], u[2];
	float ang;
	int i, k;
	
	butterfly_parms *p_parms = (butterfly_parms *)p_parms_struct;
	float *a = p_parms->a;
	float *y = p_parms->y;
	int n = p_parms->n;
//	int i = p_parms->i;
	int start = p_parms->start;
	int end = p_parms->end;
	int p = p_parms->p;
	int direction = p_parms->direction;

	for (i = start; i <= end; i++)
	{
		if (i < (n >> 1))
		{
			int o_idx = i + (n >> 1);
			
			k = i & (p - 1); // i % p
			ang = ((2 * PI * k) / (2 * p));
			omega[0] = cos(ang);
			omega[1] = ((float)direction) * sin(ang);
			// t = omega * a[i + n / 2];
			t[0] = omega[0] * a[o_idx] - omega[1] * a[o_idx + n];
			t[1] = omega[0] * a[o_idx + n] + omega[1] * a[o_idx];
			// u = a[i];
			u[0] = a[i];
			u[1] = a[i + n];

			//	y[2 * i - k] = u + t;
			y[2 * i - k] = u[0] + t[0];
			y[2 * i - k + n] = u[1] + t[1];
			//	y[2 * i - k + p] = u - t;
			y[2 * i - k + p] = u[0] - t[0];
			y[2 * i - k + p + n] = u[1] - t[1];
		}
	}
}

void fft_nrvs(int n, float *a, float *y, int direction)
{
	int p, i, k;
	int num_tasks = ((n >> 1) + (num_twiddle_threads - 1)) / num_twiddle_threads;
//	int num_tail_tasks = (n >> 1) - (num_twiddle_threads - 1) * num_tasks;

	butterfly_parms *thread_args = (butterfly_parms *)malloc(num_twiddle_threads * sizeof(butterfly_parms));
	pthread_t *thread_ids = (pthread_t *)malloc(num_twiddle_threads * sizeof(pthread_t));

//	lgn = log2(n);
//	for (i = 0; i < n; i++)
//		y[i] = a[i];

	/*
	thread_args.a = a;
	thread_args.y = y;
	thread_args.n = n;
	thread_args.direction = direction;
	*/
	for (p = 1; p <= (n / 2); p <<= 1)
	{
		//	omega_m[0] = cos((2 * PI) / m);
		//	omega_m[1] = ((float)direction) * sin((2 * PI) / m);
		//	thread_args.p = p;
		//	for (i = 0; i < (n >> 1); i++)
		for (i = 0; i < num_twiddle_threads; i++)
		{
			thread_args[i].start = i * num_tasks;
			thread_args[i].end = (i + 1) * num_tasks - 1;
		
			thread_args[i].a = a;
			thread_args[i].y = y;
			thread_args[i].n = n;
			thread_args[i].direction = direction;
			thread_args[i].p = p;
			//	thread_args[i].i = i;
			pthread_create(&thread_ids[i], NULL, &thread_butterfly, &thread_args[i]);
		}
		
		for (i = 0; i < num_twiddle_threads; i++)
		{
			pthread_join(thread_ids[i], NULL);
		}

		for (i = 0; i < n; i++)
		{
			a[i] = y[i];
			a[i + n] = y[i + n];
		}
	}

	free(thread_ids);
	free(thread_args);
}

