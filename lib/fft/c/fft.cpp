
/* Factored discrete Fourier transform, or FFT, and its inverse iFFT */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fft.h"

float v[N * 2], v1[N * 2], vout[N * 2], v1out[N * 2];

/* Print a vector of complexes as ordered pairs. */
static void print_vector(
		const char *title,
		float x[N * 2],
		int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[2 * i + 0],x[2 * i + 1]);
	putchar('\n');
	return;
}

int BitReverse(int src, int size)
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

inline int pow2(int n)
{
	return 1<<n;
}

inline int log2(int n)
{
	int i = 0;

	while (n > 1)
	{
		n /= 2;
		i++;
	}
	return i;
}

void recur_fft(int n, float (*a)[2], float (*y)[2], int direction)
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

		recur_fft(n / 2, a, y, direction);		/* FFT on even-indexed elements of v[] */
		recur_fft(n / 2, a + (n / 2), y + (n / 2), direction);		/* FFT on odd-indexed elements of v[] */

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


void iter_fft(int n, float a[N * 2], float y[N * 2], int direction)
{
	int i,j,s,d,k,m;
	float t;
	float wdr,wdi,wr,wi;
	float wtr,wti;
	float tr,ti;
	float xr,xi;

	s=log2(n);
	for(i=0;i<n;i++)
	{  
		j = BitReverse(i,s);
		y[2 * j + 0] = a[2 * i + 0];
		y[2 * j + 1] = a[2 * i + 1];
	}

	for(j=1;j<=s;j++)
	{
		d  = pow2(j);
		t = 2*PI/d;
		wdr = cos(t); wdi = ((float)direction) * sin(t);

		wr = 1; wi = 0;
		for (k=0;k<=d/2-1;k++)
		{
			for (m=k;m<=n-1;m+=d)
			{
				tr = wr*y[2 * (m + d/2) + 0] - wi*y[2 * (m + d/2) + 1];
				ti = wr*y[2 * (m + d/2) + 1] + wi*y[2 * (m + d/2) + 0];
	//			printf("r=%f, i=%f\n", tr, ti);
				xr = y[2 * m + 0]; xi = y[2 * m + 1];
	//			printf("r=%f, i=%f\n", y[2 * m + 0], y[2 * m + 1]);
				y[2 * m + 0] = xr + tr; y[2 * m + 1] = xi + ti;
				y[2 * (m+d/2) + 0] = xr - tr; y[2 * (m+d/2) + 1] = xi - ti;   
	//			printf("r=%f, i=%f\n", y[2 * m + 0], y[2 * m + 1]);
	//			printf("r=%f, i=%f\n", y[2 * (m + d/2) + 0], y[2 * (m + d/2) + 1]);
			}
			wtr = wr*wdr - wi*wdi;
			wti = wr*wdi + wi*wdr;
			wr = wtr;
			wi = wti;
		}
	}
}

int main(int argc, char *argv[])
{
//	float v[N][2], v1[N][2], vout[N][2], v1out[N][2];
	int k;

	FILE *fptr_real, *fptr_imag;

//	fptr_real = fopen("../lte/src/OFDM/testsuite/ModulationInputReal", "r");
//	fptr_imag = fopen("../lte/src/OFDM/testsuite/ModulationInputImag", "r");

	// Fill v[] with a function of known FFT:
	for(k=0; k<N; k++)
	{
		v[2 * k + 0] = 0.125*cos(2*PI*k/(float)N);
		v[2 * k + 1] = 0.125*sin(2*PI*k/(float)N);
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
	iter_fft(N, v, vout, -1);
	for (k = 0; k < N; k++)
	{
		vout[2 * k + 0] /= N;
		vout[2 * k + 1] /= N;
	}
	print_vector("iFFT", vout, N);
	iter_fft(N, vout, v, 1);
	print_vector(" FFT", v, N);

//	print_vector("Orig", v1, N);
//	fft(N, v1, v1out, -1);
//	print_vector(" FFT", v1out, N);
//	fft(N, v1out, v1, 1);
//	print_vector("iFFT", v1, N);

	return 0;
}
