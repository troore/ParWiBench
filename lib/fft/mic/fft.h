
#ifndef __FFT_H_
#define __FFT_H_

void recur_fft(int n, float (*a)[2], float (*y)[2], int direction);

void iter_fft(int n, float a[], float y[], int direction);

void fft_nrvs(int n, float *a, float *y, int direction);

#endif
