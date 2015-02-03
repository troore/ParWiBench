#include <stdio.h>
#include <math.h>
#include <memory.h>
#define pi 3.14159265358979
const int MAX_N = 256; // 最大DFT点数

//======================二进制位倒序排列函数==============================
//将整数src的二进制位按倒序排列后返回，其中size为整数src的二进制的长度
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
//=====================内部的一个求2的次幂的函数==============================
int pow2(int n)
{
	return 1<<n;
}

//========================FFT:Fast Fourier Transform==========================


//========================Iterative_FFT=======================================
//快速傅立叶变换算法的串行迭代实现
void Iterative_FFT(double *ir,double *ii,double *or,double *oi,int len)
{
	int i,j,n,d,k,m;
	double t;
	double wdr,wdi,wr,wi;
	double wtr,wti;
	double tr,ti;
	double xr,xi;

	n=pow2(len);
	for(i=0;i<n;i++)
	{  
		j = BitReverse(i,len);
		or[j] = ir[i];
		oi[j] = ii[i];
	}

	for(j=1;j<=len;j++)
	{
		d  = pow2(j);
		t = 2*pi/d;
		wdr = cos(t); wdi = sin(t);

		wr = 1; wi = 0;
		for (k=0;k<=d/2-1;k++)
		{
			for (m=k;m<=n-1;m+=d)
			{
				tr = wr*or[m + d/2] - wi*oi[m + d/2];
				ti = wr*oi[m + d/2] + wi*or[m + d/2];
				printf("r=%f, i=%f\n", tr, ti);
				xr = or[m]; xi = oi[m];
				printf("r=%f, i=%f\n", or[m], oi[m]);
				or[m] = xr + tr; oi[m] = xi + ti;
				or[m+d/2] = xr - tr; oi[m+d/2] = xi - ti;   
				printf("r=%f, i=%f\n", or[m], oi[m]);
				printf("r=%f, i=%f\n", or[m+d/2], oi[m+d/2]);
			}
			wtr = wr*wdr - wi*wdi;
			wti = wr*wdi + wi*wdr;
			wr = wtr;
			wi = wti;
		}
	}
}


//===================主函数========================

int main()
{
	double fr[4],fi[4],gr[4],gi[4];

	int i;
	memset(gr,0,sizeof(gr));
	memset(gi,0,sizeof(gi));

	//==========一组测试用的数据==================
	//==========(1,2,4,3)的傅立叶变换为(10,-3-i,0,-3+i)=========
	fr[0]=1;fi[0]=0;
	fr[1]=2;fi[1]=0;
	fr[2]=4;fi[2]=0;
	fr[3]=3;fi[3]=0;

	Iterative_FFT(fr,fi,gr,gi,2);
	printf("原始数据:n");
	for(i=0;i<4;i++)
	{
		printf("%.2f+(%.2f)in",fr[i],fi[i]);
	}
	printf("FFT:n");
	for(i=0;i<4;i++)
	{
		printf("%.2f+(%.2f)in",gr[i],gi[i]);
	}

	return 0;
}
