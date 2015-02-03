#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI	3.14159265358979323846264338327950288

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

