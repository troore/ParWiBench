#ifndef __FFT_H_
#define __FFT_H_

#define N (1 << 3)

#ifndef PI
#define PI	3.14159265358979323846264338327950288
#endif

/* function prototypes */
void fft(int n, float (*a)[2], float (*y)[2], int direction);

#endif
