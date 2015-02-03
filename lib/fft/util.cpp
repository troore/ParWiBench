
#include <stdio.h>
#include <stdlib.h>
#include "util.h"


/* Print a vector of complexes as ordered pairs. */
void print_vector(
	const char *title,
	float (*x)[2],
	int n)
{
	int i;
	printf("%s (dim=%d):", title, n);

	for (i = 0; i < n; i++ )
		printf(" %f,%f ", x[i][0], x[i][1]);
	
	putchar('\n');
}

void print_vector(
	const char *title,
	float *x,
	int n)
{
	int i;
	printf("%s (dim=%d):", title, n);

	for (i = 0; i < n; i++ )
		printf(" %f,%f ", x[i], x[i + n]);
	
	putchar('\n');
}

void print_vector(
	const char *title,
	float *x_real, float *x_imag,
	int n)
{
	int i;
	printf("%s (dim=%d):", title, n);

	for (i = 0; i < n; i++ )
		printf(" %f,%f ", x_real[i], x_imag[i]);
	
	putchar('\n');
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

int pow2(int n)
{
	return 1 << n;
}

int log2(int n)
{
	int i = 0;

	while (n > 1)
	{
		n /= 2;
		i++;
	}
	return i;
}

double gflop_counter(int n)
{
	int cnter = 0;

	cnter += log2(n) * (n / 2) * 13;

	return 1.0e-9 * cnter;
}
