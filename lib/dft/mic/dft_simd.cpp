#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "dft.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
#include <immintrin.h>
#define LEN16 16
#define PI      3.14159265358979323846264338327950288
//#define N (8)


/*
   Discrete Fourier Transform
   */

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
	__m512 reg;
	float elems[LEN16];
} zmmf_t;
#endif

void dft(int n, float a[], float y[], int direction)
{
	int k, j, l;
	float ang;
	float p[2];
/*	if(n<=16)
	{
		for (k = 0; k < n; k++)
		{
			p[0] = 0.0;
			p[1] = 0.0;
			ang = ((float)direction) * 2.0 * PI * ((float)k) / ((float)n);

			for (j = 0; j < n; j++)
			{
				p[0] += (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
				p[1] += (a[2 * j + 0] * sin(j * ang) - a[2 * j + 1] * cos(j * ang));
			}

			y[2 * k + 0] = p[0];
			y[2 * k + 1] = p[1];
		}
		return;
	}*/

#ifdef __MIC__
	zmmf_t tmp0,tmp1,result3,result0, result1, a0, a1, cos_, sin_, ang_;
	for (k = 0; k <= n-16; k+=16)
	{
		for(l=0;l<16;l++)
		{
			ang_.elems[l] = ((float)direction) * 2.0 * PI * ((float)(k+l)) / ((float)n);
			result0.elems[l] = 0;
			result1.elems[l] = 0;
		}

		for (j = 0; j < n; j++)
		{
			cos_.elems[0:16] = cos(j * ang_.elems[0:16]);
			sin_.elems[0:16] = sin(j * ang_.elems[0:16]);
/*			a0.elems[0] = a[2*j+0];
			a0.elems[1] = a0.elems[0];
			a0.elems[2:2] = a0.elems[0:2];
			a0.elems[4:4] = a0.elems[0:4];
			a0.elems[8:8] = a0.elems[0:8];
			a1.elems[0] = a[2*j+1];
			a1.elems[1] = a1.elems[0];
			a1.elems[2:2] = a1.elems[0:2];
			a1.elems[4:4] = a1.elems[0:4];
			a1.elems[8:8] = a1.elems[0:8];
*/
			a0.elems[0] = a[2*j+0];
			a0.elems[1] = a[2*j+0];
			a0.elems[2] = a[2*j+0];
			a0.elems[3] = a[2*j+0];
			a0.elems[4] = a[2*j+0];
			a0.elems[5] = a[2*j+0];
			a0.elems[6] = a[2*j+0];
			a0.elems[7] = a[2*j+0];
			a0.elems[8] = a[2*j+0];
			a0.elems[9] = a[2*j+0];
			a0.elems[10] = a[2*j+0];
			a0.elems[11] = a[2*j+0];
			a0.elems[12] = a[2*j+0];
			a0.elems[13] = a[2*j+0];
			a0.elems[14] = a[2*j+0];
			a0.elems[15] = a[2*j+0];
			a1.elems[0] = a[2*j+1];
			a1.elems[1] = a[2*j+1];
			a1.elems[2] = a[2*j+1];
			a1.elems[3] = a[2*j+1];
			a1.elems[4] = a[2*j+1];
			a1.elems[5] = a[2*j+1];
			a1.elems[6] = a[2*j+1];
			a1.elems[7] = a[2*j+1];
			a1.elems[8] = a[2*j+1];
			a1.elems[9] = a[2*j+1];
			a1.elems[10] = a[2*j+1];
			a1.elems[11] = a[2*j+1];
			a1.elems[12] = a[2*j+1];
			a1.elems[13] = a[2*j+1];
			a1.elems[14] = a[2*j+1];
			a1.elems[15] = a[2*j+1];
			tmp0.reg  = _mm512_fnmadd_ps(a1.reg,sin_.reg,result0.reg);
			result0.reg = _mm512_fmadd_ps(a0.reg, cos_.reg,tmp0.reg);
			tmp1.reg  = _mm512_fnmadd_ps(a1.reg,cos_.reg,result1.reg);
			result1.reg = _mm512_fmadd_ps(a0.reg, sin_.reg,tmp1.reg);
		}
		for(l=0;l<16;l++)
		{
			y[2 * (k+l) + 0] = result0.elems[l];
			y[2 * (k+l) + 1] = result1.elems[l];
		}
	}

	for (k=k; k < n; k++)
	{
		p[0] = 0.0;
		p[1] = 0.0;
		ang = ((float)direction) * 2.0 * PI * ((float)k) / ((float)n);




		for (j = 0; j < n; j++)
		{
			p[0] += (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
			p[1] += (a[2 * j + 0] * sin(j * ang) - a[2 * j + 1] * cos(j * ang));
		}
		y[2 * k + 0] = p[0];
		y[2 * k + 1] = p[1];

	}
#endif
}
