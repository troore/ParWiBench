#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#define PI	3.14159265358979323846264338327950288

extern int num_threads;

/*
  Discrete Fourier Transform
*/
void dft(int n, float a[], float y[], int direction)
{
	int k, j;
	float ang;
//	float p[2];
	float p0, p1;

	omp_set_num_threads(num_threads);

#pragma omp parallel shared(n, a, y)
	{
//#pragma omp for private(k, p, j, ang)
#pragma omp for private(k, p0, p1, j, ang)
		for (k = 0; k < n; k++)
		{
			//	p[0] = 0.0;
			//	p[1] = 0.0;
			p0 = 0.0;
			p1 = 0.0;
			ang = ((float)direction) * 2.0 * PI * ((float)k) / ((float)n);
			/*
			for (j = 0; j < n; j++)
			{
				p[0] += (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
				p[1] += (a[2 * j + 0] * sin(j * ang) + a[2 * j + 1] * cos(j * ang));
			}
			*/
#pragma omp parallel for private(j) reduction(+:p0)
			for (j = 0; j < n; j++)
			{
				p0 = p0 + (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
			}
#pragma omp parallel for private(j) reduction(+:p1)
			for (j = 0; j < n; j++)
			{
				p1 = p1 + (a[2 * j + 0] * sin(j * ang) + a[2 * j + 1] * cos(j * ang));
			}
			//	y[2 * k + 0] = p[0];
			//	y[2 * k + 1] = p[1];
			y[2 * k + 0] = p0;
			y[2 * k + 1] = p1;
		}
	}
}

