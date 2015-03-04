
/* Factored discrete Fourier transform, or FFT, and its inverse iFFT */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

#include "fft.h"
#include "util.h"

#define PI	3.14159265358979323846264338327950288

#define LEN16	16

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
	        __m512 reg;
	        float elems[LEN16];
} zmmf_t;
#endif

void fft_nrvs(int n, float *a, float *y, int direction)
{
	int p, i, k;
	int lgn;
	float omega_m[2], omega[2];
	float t[2], u[2];
	float ang;

//	lgn = log2(n);
//	for (i = 0; i < n; i++)
//		y[i] = a[i];

	for (p = 1; p <= (n / 2); p <<= 1)
	{
		//	omega_m[0] = cos((2 * PI) / m);
		//	omega_m[1] = ((float)direction) * sin((2 * PI) / m);
		for (i = 0; i < (n >> 1); i+=16)
		{
			int o_idx = i + (n >> 1);
			zmmf_t omg0,omg1,a_0,a_1,t_0,t_1,u_0,u_1,tmp_1,result_1,result_2,result_3,result_4;
			k = i & (p - 1); // i % p
			ang = ((2 * PI * k) / (2 * p));
			omega[0] = cos(ang);
			omega[1] = ((float)direction) * sin(ang);
			for(int j=0;j<16;j++)
			{
				k = (i + j) & (p - 1); // i % p
				ang = ((2 * PI * k) / (2 * p));
				omega[0] = cos(ang);
				omega[1] = ((float)direction) * sin(ang);
				omg0.elems[j] = omega[0];
				omg1.elems[j] = omega[1];
			}
			a_0.elems[0:16] = a[o_idx:16];
			a_1.elems[0:16] = a[(o_idx+n):16];
			// t = omega * a[i + n / 2];
			tmp_1.reg = _mm512_mul_ps(omg0.reg,a_0.reg);
			t_0.reg = _mm512_fnmadd_ps(omg1.reg,a_1.reg,tmp_1.reg);
			tmp_1.reg = _mm512_mul_ps(omg0.reg,a_1.reg);
			t_1.reg = _mm512_fmadd_ps(omg1.reg,a_0.reg,tmp_1.reg);
			u_0.elems[0:16] = a[i:16];
			u_1.elems[0:16] = a[(i+n):16];
			result_1.reg = _mm512_add_ps(u_0.reg,t_0.reg);
			result_2.reg = _mm512_add_ps(u_1.reg,t_1.reg);
			result_3.reg = _mm512_sub_ps(u_0.reg,t_0.reg);
			result_4.reg = _mm512_sub_ps(u_1.reg,t_1.reg);
			for(int j=0;j<16;j++)
			{
				k = (i + j) & (p - 1);
				y[2 * (i + j) - k] = result_1.elems[j];
				y[2 * (i + j) - k + n] = result_2.elems[j];
				y[2 * (i + j) - k + p] = result_3.elems[j];
				y[2 * (i + j) - k + p + n] = result_4.elems[j];
			}/*
			for(int j=0;j<16;j++)
			{
				k = (i + j) & (p - 1); // i % p
				                                ang = ((2 * PI * k) / (2 * p));                         
								                                omega[0] = cos(ang);                                                                                                    
												                                omega[1] = ((float)direction) * sin(ang);                               

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
			}*/
		}
		
		for (i = 0; i < n; i++)
		{
			a[i] = y[i];
			a[i + n] = y[i + n];
		}
	}
}

