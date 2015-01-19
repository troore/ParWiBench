
/* Factored discrete Fourier transform, or FFT, and its inverse iFFT */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>
#include "fft.h"
#include <sys/time.h>

#define LEN16 16
#define N (1 << 5)
#define PI	3.14159265358979323846264338327950288

//float v[N * 2], v1[N * 2], vout[N * 2], v1out[N * 2];
float v[2 * N], v1[2 * N], vout[2 * N], v1out[2 * N];


/* Print a vector of complexes as ordered pairs. */
static void print_vector(
		const char *title,
		float *x,
		int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
#ifdef DEBUG
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[2 * i + 0],x[2 * i + 1]);
#else
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[i], x[i + n]);
#endif
	putchar('\n');
	return;
}

static int BitReverse(int src, int size)
{
	int tmp = src;
	int des = 0;
	for (int i=size-1; i>=0; i--)
	{
		des = ((tmp & 1) << i) | des;
		tmp = tmp >> 1;
	}
	return des;
}

static inline int pow2(int n)
{
	return 1<<n;
}

static inline int log2(int n)
{
	int i = 0;

	while (n > 1)
	{
		n /= 2;
		i++;
	}
	return i;
}

void fft_recur(int n, float (*a)[2], float (*y)[2], int direction)
{
    if (n > 1)
    {
        /* otherwise, do nothing and return */
        int k;
        float z[2], w[2];
        
        for (k = 0; k < n / 2; k++)
        {
            y[k][0] = a[2 * k][0];
            y[k][1] = a[2 * k][1];
            y[k + n / 2][0] = a[2 * k + 1][0];
            y[k + n / 2][1] = a[2 * k + 1][1];
        }
        for (k = 0; k < n; k++)
        {
            a[k][0] = y[k][0];
            a[k][1] = y[k][1];
        }
        
        fft_recur(n / 2, a, y, direction);		/* FFT on even-indexed elements of v[] */
        fft_recur(n / 2, a + (n / 2), y + (n / 2), direction);		/* FFT on odd-indexed elements of v[] */
        
        for (k = 0; k < n; k++)
        {
            a[k][0] = y[k][0];
            a[k][1] = y[k][1];
        }
        
        for (k = 0; k < (n / 2); k++)
        {
            w[0] = cos(2 * PI * k / (float)n);
            w[1] = ((float)direction) * sin(2 * PI * k / (float)n);
            z[0] = w[0] * a[n / 2 + k][0] - w[1] * a[n / 2 + k][1];	/* Re(w * y[n / 2 + k]) */
            z[1] = w[0] * a[n / 2 + k][1] + w[1] * a[n / 2 + k][0];	/* Im(w * y[n / 2 + k]) */
            y[k][0] = a[k][0] + z[0];
            y[k][1] = a[k][1] + z[1];
            y[k + n / 2][0] = a[k][0] - z[0];
            y[k + n / 2][1] = a[k][1] - z[1];
        }
    }
}

void fft_iter(int n, float a[N * 2], float y[N * 2], int direction)
{
    int i, j, s, d, k, m;
    float t;
    float wdr, wdi, wr, wi;
    float wtr, wti;
    float tr, ti;
    float xr, xi;
    
    s = log2(n);
    for (i = 0; i < n; i++)
    {
        j = BitReverse(i, s);
        //	y[2 * j + 0] = a[2 * i + 0];
        //	y[2 * j + 1] = a[2 * i + 1];
        y[j] = a[i];
        y[j + n] = a[i + n];
    }
    
    for (j = 1; j <= s; j++)
    {
        d  = pow2(j);
        t = (2 * PI) / d;
        wdr = cos(t);
        wdi = ((float)direction) * sin(t);
        
        wr = 1; wi = 0;
        for (k = 0; k < (d / 2); k++)
        {
            for (m = k; m < n; m += d)
            {
                /*
                 tr = wr*y[2 * (m + d/2) + 0] - wi*y[2 * (m + d/2) + 1];
                 ti = wr*y[2 * (m + d/2) + 1] + wi*y[2 * (m + d/2) + 0];
                 */
                tr = wr * y[(m + d / 2)] - wi * y[(m + d / 2) + n];
                ti = wr * y[(m + d / 2) + n] + wi * y[(m + d/2)];
                //			printf("r=%f, i=%f\n", tr, ti);
                xr = y[m];
                xi = y[m + n];
                //			printf("r=%f, i=%f\n", y[2 * m + 0], y[2 * m + 1]);
                /*
                 y[2 * m + 0] = xr + tr;
                 y[2 * m + 1] = xi + ti;
                 */
                y[m] = xr + tr; y[m + n] = xi + ti;
                /*
                 y[2 * (m+d/2) + 0] = xr - tr;
                 y[2 * (m+d/2) + 1] = xi - ti;
                 */
                y[m + d / 2] = xr - tr;
                y[m + d / 2 + n] = xi - ti;
                //			printf("r=%f, i=%f\n", y[2 * m + 0], y[2 * m + 1]);
                //			printf("r=%f, i=%f\n", y[2 * (m + d/2) + 0], y[2 * (m + d/2) + 1]);
            }
            wtr = wr * wdr - wi * wdi;
            wti = wr * wdi + wi * wdr;
            wr = wtr;
            wi = wti;
        }
    }
}


void fft_iter(int n, float *a_real, float *a_imag, float *y_real, float *y_imag, int direction)
{
    int i, j, s, d, k, m;
    float t;
    float wdr, wdi, wr, wi;
    float wtr, wti;
    float tr, ti;
    float xr, xi;
    
    s = log2(n);
    for (i = 0; i < n; i++)
    {  
        j = BitReverse(i, s);
        y_real[j] = a_real[i];
        y_imag[j] = a_imag[i];
    }
    
    for (j = 1; j <= s; j++)
    {
        d  = pow2(j);
        t = (2 * PI) / d;
        wdr = cos(t);
        wdi = ((float)direction) * sin(t);
        
        wr = 1; wi = 0;
        for (k = 0; k < (d / 2); k++)
        {
            for (m = k; m < n; m += d)
            {
                tr = wr * y_real[(m + d / 2)] - wi * y_imag[(m + d / 2)];
                ti = wr * y_imag[(m + d / 2)] + wi * y_real[(m + d / 2)];
                xr = y_real[m];
                xi = y_imag[m];
                
                y_real[m] = xr + tr;
                y_imag[m] = xi + ti;
                
                y_real[m + d / 2] = xr - tr;
                y_imag[m + d / 2] = xi - ti;  
            }
            wtr = wr * wdr - wi * wdi;
            wti = wr * wdi + wi * wdr;
            wr = wtr;
            wi = wti;
        }
    }
}

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
/*
int main(int argc, char *argv[])
{
//	float v[N][2], v1[N][2], vout[N][2], v1out[N][2];
	int k;

	FILE *fptr_real, *fptr_imag;

//	fptr_real = fopen("../lte/src/OFDM/testsuite/ModulationInputReal", "r");
//	fptr_imag = fopen("../lte/src/OFDM/testsuite/ModulationInputImag", "r");

	// Fill v[] with a function of known FFT:
	for (k = 0; k < N; k++)
	{
#ifdef DEBUG
		v[2 * k + 0] = 0.125*cos(2*PI*k/(float)N);
		v[2 * k + 1] = 0.125*sin(2*PI*k/(float)N);
#else
		v[k] = 0.125 * cos((2 * PI * k) / (float)N);
		v[k + N] = 0.125 * sin((2 * PI * k ) / (float)N);
#endif
	//	v1[k][0] =  0.3*cos(2*PI*k/(float)N);
	//	v1[k][1] = -0.3*sin(2*PI*k/(float)N);

	//	fscanf(fptr_real, "%f", &(v[k][0]));
	//	fscanf(fptr_imag, "%f", &(v[k][1]));
	}

//	v[0]=1;v[1]=0;
//	v[2]=2;v[3]=0;
//	v[4]=4;v[5]=0;
//	v[6]=3;v[7]=0;

	print_vector("Orig", v, N);
#ifdef DEBUG
	iter_fft(N, v, vout, -1);
#else
	fft_nrvs(N, v, vout, -1);
#endif

#ifdef DEBUG
	for (k = 0; k < N; k++)
	{
		vout[2 * k + 0] /= N;
		vout[2 * k + 1] /= N;
	}
#else
	for (k = 0; k < N; k++)
	{
		vout[k] /= N;
		vout[k + N] /= N;
	}
#endif
	print_vector("iFFT", vout, N);
	double ttime,tbegin;
	tbegin = dtime();
for(int i=0;i<100000;i++)
#ifdef DEBUG
	iter_fft(N, vout, v, 1);
#else
	fft_nrvs(N, vout, v, 1);
#endif
	ttime = dtime();
	printf("whole time is %f\n",ttime - tbegin);
	print_vector(" FFT", v, N);

//	print_vector("Orig", v1, N);
//	fft(N, v1, v1out, -1);
//	print_vector(" FFT", v1out, N);
//	fft(N, v1out, v1, 1);
//	print_vector("iFFT", v1, N);

	return 0;
}
*/
