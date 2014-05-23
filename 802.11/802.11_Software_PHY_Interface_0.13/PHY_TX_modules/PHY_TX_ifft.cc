
//6bit的按位反序，用于第一级交换数组顺序 
int Bit_inv(int i)
{
	int k,ans;
	k=32*(i&0x01);
	ans=k;
	k=16*((i>>1)&0x01);
	ans=ans+k;
	k=8*((i>>2)&0x01);
	ans=ans+k;
	k=4*((i>>3)&0x01);
	ans=ans+k;
	k=2*((i>>4)&0x01);
	ans=ans+k;
	k=(i>>5)&0x01;
	ans=ans+k;
	return ans;
}

float FFT_sin(int i)
{
	float pi;
	pi=3.1415926;
	return sin(pi/32.0*i);
}

float FFT_cosaddsin(int i)
{
	float pi;
	pi=3.1415926;
	return cos(pi/32.0*i)+sin(pi/32.0*i);
}

float FFT_cossubsin(int i)
{
	float pi;
	pi=3.1415926;
	return cos(pi/32.0*i)-sin(pi/32.0*i);
}



//加法，加完后重新变成12位小数，DSP1是上半部分，DSP2是下半部分 
Complex FFT_DSP_1(Complex a, Complex b, int k)
{
	Complex ans;
	ans.real = a.real + b.real * FFT_cosaddsin(k) + (b.imag-b.real) * FFT_sin(k);
	ans.imag = a.imag + b.imag * FFT_cossubsin(k) + (b.imag-b.real) * FFT_sin(k);
	return ans;
}
Complex FFT_DSP_2(Complex a, Complex b, int k)
{
	Complex ans;
	ans.real = a.real - b.real * FFT_cosaddsin(k) - (b.imag-b.real) * FFT_sin(k);
	ans.imag = a.imag - b.imag * FFT_cossubsin(k) - (b.imag-b.real) * FFT_sin(k);
	return ans;
}

int IFFT_64(Complex *din, Complex *dout)
{
	//储存每一级的运算结果
	Complex xb[64];
	Complex x0[64];
	Complex x1[64];
	Complex x2[64];
	Complex x3[64];
	Complex x4[64];
	Complex x5[64];
	Complex x6[64];	
	
	int i,k;
	
	//shift移位
	for(i=32; i<64; i++) 
		xb[i-32]=din[i];
	for(i=0; i<32; i++)
		xb[i+32]=din[i]; 
		
	
	
	
	//第一步：交换次序 
	for(i=0;i<64;i++)
	{
		x0[i]=xb[Bit_inv(i)];
	}
	
	//第一级蝶形变换x0->x1
	for(i=0;i<64;i=i+2)
	{
		x1[i]  =FFT_DSP_1(x0[i], x0[i+1], 0);
		x1[i+1]=FFT_DSP_2(x0[i], x0[i+1], 0);
	}
	
	//第二级蝶形变换x1->x2
	for(i=0;i<64;i=i+4)
	{
		x2[i]  =FFT_DSP_1(x1[i],   x1[i+2], 0);
		x2[i+2]=FFT_DSP_2(x1[i],   x1[i+2], 0);
		x2[i+1]=FFT_DSP_1(x1[i+1], x1[i+3], 16);
		x2[i+3]=FFT_DSP_2(x1[i+1], x1[i+3], 16);
	}

	
	//第三级蝶形变换x2->x3
	for(i=0;i<64;i=i+8)
	{
		for(k=0;k<4;k++)
		{
			x3[i+k] = FFT_DSP_1(x2[i+k], x2[i+4+k], (8*k));
			x3[i+4+k] = FFT_DSP_2(x2[i+k], x2[i+4+k], (8*k));
		}
	}
	
	//第四级蝶形变换 
	for(i=0;i<64;i=i+16)
	{
		for(k=0;k<8;k++)
		{
			x4[i+k] = FFT_DSP_1(x3[i+k], x3[i+8+k], (4*k));
			x4[i+8+k] = FFT_DSP_2(x3[i+k], x3[i+8+k], (4*k));
		}
	}
	
	//第五级蝶形变换
	for(i=0;i<64;i=i+32)
	{
		for(k=0;k<16;k++)
		{
			x5[i+k] = FFT_DSP_1(x4[i+k], x4[i+16+k], (2*k));
			x5[i+16+k] = FFT_DSP_2(x4[i+k], x4[i+16+k], (2*k));
		}
	}
	
	//第六级蝶形变换
	for(k=0;k<32;k++)
	{
		x6[k] = FFT_DSP_1(x5[k], x5[32+k], k);
		x6[32+k] = FFT_DSP_2(x5[k], x5[32+k], k);
	}
	
	//输出结果
	dout[0]=Complex_mul_const(x6[0], 1/64.0);
	for(i=1;i<64;i++)
	{
		dout[i]=Complex_mul_const(x6[64-i], 1/64.0);
	}
	return 1; 
}



int IFFT(Complex *Input, Complex *Output, int len_symbol) 
{
	int i;
	for(i=0; i<len_symbol; i++)
	{
		IFFT_64(Input+64*i, Output+64*i); 
	}
	return 1; 
} 
