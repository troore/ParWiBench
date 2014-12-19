
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "dft.h"
#include <immintrin.h>
#define LEN16 16
#define PI      3.14159265358979323846264338327950288
//#define N (8)


//float v[N * 2], v1[N * 2], vout[N * 2], v1out[N * 2];

/* Print a vector of complexes as ordered pairs. */
/*
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
*/


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
#ifdef __MIC__
    zmmf_t tmp0,tmp1,result3,result0, result1, a0, a1, cos_, sin_, ang_;
    printf("MIC\n");
	for (k = 0; k < n-16; k+=16)
	{
//		p[0] = 0.0;
//		p[1] = 0.0;
        for(l=0;l<16;l++)
        {
            ang_.elems[l] = ((float)direction) * 2.0 * PI * ((float)(k+l)) / ((float)n);
            //a0.elems[l] = a[2 * (k+l) + 0];
            //a1.elems[l] = a[2 * (k+l) + 1];
            result0.elems[l] = 0;
            result1.elems[l] = 0;
//	    result3.elems[l] = 0;
        }

	for (j = 0; j < n; j++)
	{
            cos_.elems[0:16] = cos(j * ang_.elems[0:16]);
            sin_.elems[0:16] = sin(j * ang_.elems[0:16]);
		a0.elems[0] = a[2*j+0];
		a1.elems[0] = a[2*j+1];
		a0.elems[1] = a[2*j+0];
		a1.elems[1] = a[2*j+1];
		a0.elems[2] = a[2*j+0];
		a1.elems[2] = a[2*j+1];
		a0.elems[3] = a[2*j+0];
		a1.elems[3] = a[2*j+1];
		a0.elems[4] = a[2*j+0];
		a1.elems[4] = a[2*j+1];
		a0.elems[5] = a[2*j+0];
		a1.elems[5] = a[2*j+1];
		a0.elems[6] = a[2*j+0];
		a1.elems[6] = a[2*j+1];
		a0.elems[7] = a[2*j+0];
		a1.elems[7] = a[2*j+1];
		a0.elems[8] = a[2*j+0];
		a1.elems[8] = a[2*j+1];
		a0.elems[9] = a[2*j+0];
		a1.elems[9] = a[2*j+1];
		a0.elems[10] = a[2*j+0];
		a1.elems[10] = a[2*j+1];
		a0.elems[11] = a[2*j+0];
		a1.elems[11] = a[2*j+1];
		a0.elems[12] = a[2*j+0];
		a1.elems[12] = a[2*j+1];
		a0.elems[13] = a[2*j+0];
		a1.elems[13] = a[2*j+1];
		a0.elems[14] = a[2*j+0];
		a1.elems[14] = a[2*j+1];
		a0.elems[15] = a[2*j+0];
		a1.elems[15] = a[2*j+1];
//            result3.elems[0:16] += a0.elems[0:16]*cos_.elems[0:16] - a1.elems[0:16]*sin_.elems[0:16];//intrinsic
            //result1.elems[0:16] += a0.elems[0:16]*sin_.elems[0:16] - a1.elems[0:16]*cos_.elems[0:16];
            
	    tmp0.reg  = _mm512_fnmadd_ps(a1.reg,sin_.reg,result0.reg);
            result0.reg = _mm512_fmadd_ps(a0.reg, cos_.reg,tmp0.reg);
            tmp1.reg  = _mm512_fnmadd_ps(a1.reg,cos_.reg,result1.reg);
	    result1.reg = _mm512_fmadd_ps(a0.reg, sin_.reg,tmp1.reg);
//	    printf("%f %f %f %f %f %f %f\n",a0.elems[0], cos_.elems[0], a1.elems[0], sin_.elems[0], result0.elems[0], result3.elems[0], tmp0.elems[0]);
	}
//        for(l=0;l<16;l++)
//		if(result0.elems[l]!=result3.elems[l]) printf("wrong here");
        for(l=0;l<16;l++)
        {
            y[2 * (k+l) + 0] = result0.elems[l];
            y[2 * (k+l) + 1] = result1.elems[l];
//	    printf("%f %f ", result0.elems[l],result1.elems[l]);
        }
//	if(k==0) printf("%f %f\n",y[k],y[k+1]);	
	}
//	printf("\n");
/*	for (k = 0; k < n; k++)
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
		//printf("%f %f ", p[0], p[1]);
		if(k==0) printf("%f %f\n",y[k],y[k+1]);
	}
*/	

    for (; k < n; k++)
    {
        p[0] = 0.0;
        p[1] = 0.0;
        ang = ((float)direction) * 2.0 * PI * ((float)k) / ((float)n);

        
        
        //y[2 * k + 0] = p[0];
        //y[2 * k + 1] = p[1];
        
        for (j = 0; j < n; j++)
        {
            p[0] += (a[2 * j + 0] * cos(j * ang) - a[2 * j + 1] * sin(j * ang));
            p[1] += (a[2 * j + 0] * sin(j * ang) - a[2 * j + 1] * cos(j * ang));
        }
        y[2 * k + 0] = p[0];
        y[2 * k + 1] = p[1];
        
    }

#else
    	printf("no mic");
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

#endif
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
