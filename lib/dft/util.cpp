
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void print_vector(const char *title, float *x, int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[2 * i + 0], x[2 * i + 1]);
	putchar('\n');
}

double gflop_counter(int n)
{
	int cnter = 0;

	cnter = 8 * n * n;

	return 1.0e-9 * cnter;
}
