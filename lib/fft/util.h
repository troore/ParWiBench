#ifndef __UTIL_H_
#define __UTIL_H_

void print_vector(const char *title, float (*x)[2], int n);
void print_vector(const char *title, float *x, int n);
void print_vector(const char *title, float *x_real, float *x_imag, int n);

int BitReverse(int src, int size);
int pow2(int n);
int log2(int n);

double gflop_counter(int n);

#endif
