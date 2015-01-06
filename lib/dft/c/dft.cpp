#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI	3.14159265358979323846264338327950288

#define N (1 << 5)

float v[N * 2], v1[N * 2], vout[N * 2], v1out[N * 2];
/* Print a vector of complexes as ordered pairs. */

static void
print_vector(
	const char *title,
	float *x,
	int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[2 * i + 0], x[2 * i + 1]);
	putchar('\n');
}
   
/*
  Discrete Fourier Transform
*/
void dft(int n, float a[], float y[], int direction)
{
	int k, j;
	float ang;
	float p[2];
	for (k = 0; k < n; k++)
	{
		p[0] = 0.0;
		p[1] = 0.0;
		ang = ((float)direction) * 2.0 * PI * ((float)k) / ((float)n);
		for (j = 0; j < n; j++)
		{
			p[0] += (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
			p[1] += (a[2 * j + 0] * sin(j * ang) + a[2 * j + 1] * cos(j * ang));
		}
		y[2 * k + 0] = p[0];
		y[2 * k + 1] = p[1];
	}
}

/*
int main(int argc, char *argv[])
{
	int k;
// Fill v[] with a function of known FFT:
	for (k = 0; k < N; k++)
	{
		v[2 * k + 0] = 0.125*cos(2*PI*k/(float)N);
		v[2 * k + 1] = 0.125*sin(2*PI*k/(float)N);
// v1[k][0] = 0.3*cos(2*PI*k/(float)N);
// v1[k][1] = -0.3*sin(2*PI*k/(float)N);
// fscanf(fptr_real, "%f", &(v[k][0]));
// fscanf(fptr_imag, "%f", &(v[k][1]));
	}
// v[0]=1;v[1]=0;
// v[2]=2;v[3]=0;
// v[4]=4;v[5]=0;
// v[6]=3;v[7]=0;
	print_vector("Orig", v, N);
	dft(N, v, vout, -1);
	for (k = 0; k < N; k++)
	{
		vout[2 * k + 0] /= N;
		vout[2 * k + 1] /= N;
	}
	print_vector("iFFT", vout, N);
	dft(N, vout, v, 1);
	print_vector(" FFT", v, N);
	return 0;
}
*/
