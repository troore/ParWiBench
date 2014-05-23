void Init_L_sg(int *L_sg)
{
	//={0,0,0,0,0,0,  1,1,-1,-1,1,  1,-1,1,-1,1,  1,1,1,1,1,  -1,-1,1,1,-1,  1,-1,1,1,1,  1,0,1,-1,-1,  1,1,-1,1,-1,  1,-1,-1,-1,-1,  -1,1,1,-1,-1,  1,-1,1,-1,1,  1,1,1,  0,0,0,0,0}

	L_sg[0]=0;L_sg[1]=0;L_sg[2]=0;L_sg[3]=0;L_sg[4]=0;L_sg[5]=0;

	L_sg[6]=1;L_sg[7]=1;L_sg[8]=-1;L_sg[9]=-1;L_sg[10]=1;
	L_sg[11]=1;L_sg[12]=-1;L_sg[13]=1;L_sg[14]=-1;L_sg[15]=1;
	L_sg[16]=1;L_sg[17]=1;L_sg[18]=1;L_sg[19]=1;L_sg[20]=1;
	L_sg[21]=-1;L_sg[22]=-1;L_sg[23]=1;L_sg[24]=1;L_sg[25]=-1;
	L_sg[26]=1;L_sg[27]=-1;L_sg[28]=1;L_sg[29]=1;L_sg[30]=1;

	L_sg[31]=1;L_sg[32]=0;L_sg[33]=1;L_sg[34]=-1;L_sg[35]=-1;
	L_sg[36]=1;L_sg[37]=1;L_sg[38]=-1;L_sg[39]=1;L_sg[40]=-1;
	L_sg[41]=1;L_sg[42]=-1;L_sg[43]=-1;L_sg[44]=-1;L_sg[45]=-1;
	L_sg[46]=-1;L_sg[47]=1;L_sg[48]=1;L_sg[49]=-1;L_sg[50]=-1;
	L_sg[51]=1;L_sg[52]=-1;L_sg[53]=1;L_sg[54]=-1;L_sg[55]=1;

	L_sg[56]=1;L_sg[57]=1;L_sg[58]=1;
	L_sg[59]=0;L_sg[60]=0;L_sg[61]=0;L_sg[62]=0;L_sg[63]=0;
}

//channel estimation of preamble,
//Return the channel value.
int Channel_estimation_s(Complex *Input, Complex *Output, Complex *Channel, int per_preamble)
{
	int i, n, j;
	
	int n_in, n_out;
	n_in = 0;
	n_out = 0;
	
	int L_sg[64];
	Init_L_sg(L_sg);
	
	int x[8];
	x[7] = 1;
	x[6] = 1;
	x[5] = 1;
	x[4] = 1;
	x[3] = 1;
	x[2] = 1;
	x[1] = 1;
	x[0] = 0;
	
	float Xa, Xb;
	Xa = per_preamble / 100.0;
	Xb = 1 - Xa;
	
	Complex Channel1[64];
	Complex Channel2[64];

	
	Complex Channelp[64];
	Complex Channelf[64];
	
	Complex num_21, num_7, num7, num21;
	Complex dis_a, dis_b, dis_c;
	Complex number[64];
	
	for(i=0; i<64; i++)
	{
		if(L_sg[i] == 0)
			n_in++;
		else if(L_sg[i] == 1)
			Channel1[i] = Input[n_in++];
		else
		{
			Channel1[i].real = 0.0 - Input[n_in].real;
			Channel1[i].imag = 0.0 - Input[n_in].imag;
			n_in++;
		}
	}
	
	for(i=0; i<64; i++)
	{
		if(L_sg[i] == 0)
			n_in++;
		else if(L_sg[i] == 1)
		{
			Channel2[i] = Input[n_in++];
			Channel[i] = Complex_mul_const(Complex_add(Channel1[i], Channel2[i]), 0.5);
		}
		else
		{
			Channel2[i].real = 0.0 - Input[n_in].real;
			Channel2[i].imag = 0.0 - Input[n_in].imag;
			Channel[i] = Complex_mul_const(Complex_add(Channel1[i], Channel2[i]), 0.5);
			n_in++;
		}
	}
	
	
	
	x[0] = (x[7] + x[4]) % 2;
	for(j=7; j>0; j--)
	{
		x[j] = x[j-1];
	}
	
	
	
	n_in++;
	n_in++;
	n_in++;
	n_in++;
	n_in++;
	n_in++;
	for(i=-26; i<=26; i++)
	{
		if(i==-21) num_21 = Input[n_in++];
		else if(i==-7) num_7 = Input[n_in++];
		else if(i==7) num7 = Input[n_in++];
		else if(i==21) num21 = Input[n_in++];
		else number[i+32] = Input[n_in++];
	}
	n_in++;
	n_in++;
	n_in++;
	n_in++;
	n_in++;
	
	
	if(x[0] == 0)
		num21 = Complex_init(0.0-num21.real, 0.0-num21.imag);
	else
	{
		num_21 = Complex_init( (0.0-num_21.real) , (0.0-num_21.imag) );
		num_7 = Complex_init( (0.0-num_7.real) , (0.0-num_7.imag) );
		num7 = Complex_init( (0.0-num7.real) , (0.0-num7.imag) );
	}	
	
	
	dis_a = Complex_mul_const(Complex_sub(num_7, num_21), 0.071429);
	dis_b = Complex_mul_const(Complex_sub(num7, num_7), 0.071429);
	dis_c = Complex_mul_const(Complex_sub(num21, num7), 0.071429);
	
	for(n=6; n<=24; n++)
	{
		if(n!=11)
		{
			Channelp[n] = Complex_add(num_21, Complex_mul_const(dis_a, (n-11.0)));
			Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
			Output[n_out++] = Complex_div(number[n], Channelf[n]);
		}
	}
	
	
	for(n=26; n<=38; n++)
	{
		if(n!=32)
		{
			Channelp[n] = Complex_add(num_7, Complex_mul_const(dis_b, (n-25.0)));
			Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
			Output[n_out++] = Complex_div(number[n], Channelf[n]);
		}
	}
	
	for(n=40; n<=58; n++)
	{
		if(n!=53)
		{
			Channelp[n] = Complex_add(num7, Complex_mul_const(dis_c, (n-39.0)));
			Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
			Output[n_out++] = Complex_div(number[n], Channelf[n]);
		}
	}
	
	
	
	return 1;
}


int Channel_estimation_d(Complex *Input, Complex *Output, Complex *Channel, int len_symbol, int per_preamble)
{
	int i, n, j, k;
	
	int n_in, n_out;
	n_in = 0;
	n_out = 0;
	
	int x[8];
	x[7] = 1;
	x[6] = 1;
	x[5] = 1;
	x[4] = 1;
	x[3] = 1;
	x[2] = 1;
	x[1] = 1;
	x[0] = 0;
	
	float Xa, Xb;
	Xa = per_preamble / 100.0;
	Xb = 1 - Xa;
	
	Complex Channel1[64];
	Complex Channel2[64];
	
	Complex Channelp[64];
	Complex Channelf[64];
	
	Complex num_21, num_7, num7, num21;
	Complex dis_a, dis_b, dis_c;
	Complex number[64];
	
	
	
	x[0] = (x[7] + x[4]) % 2;
	for(j=7; j>0; j--)
	{
		x[j] = x[j-1];
	}
	
	for(k=0; k<len_symbol-1; k++)
	{
	
		x[0] = (x[7] + x[4]) % 2;
		for(j=7; j>0; j--)
		{
			x[j] = x[j-1];
		}
	
		n_in++;
		n_in++;
		n_in++;
		n_in++;
		n_in++;
		n_in++;
		for(i=-26; i<=26; i++)
		{
			if(i==-21) num_21 = Input[n_in++];
			else if(i==-7) num_7 = Input[n_in++];
			else if(i==7) num7 = Input[n_in++];
			else if(i==21) num21 = Input[n_in++];
			else number[i+32] = Input[n_in++];
		}
		n_in++;
		n_in++;
		n_in++;
		n_in++;
		n_in++;
		
		
		if(x[0] == 0)
			num21 = Complex_init(0.0-num21.real, 0.0-num21.imag);
		else
		{
			num_21 = Complex_init( (0.0-num_21.real) , (0.0-num_21.imag) );
			num_7 = Complex_init( (0.0-num_7.real) , (0.0-num_7.imag) );
			num7 = Complex_init( (0.0-num7.real) , (0.0-num7.imag) );
		}	
		
		
		dis_a = Complex_mul_const(Complex_sub(num_7, num_21), 0.071429);
		dis_b = Complex_mul_const(Complex_sub(num7, num_7), 0.071429);
		dis_c = Complex_mul_const(Complex_sub(num21, num7), 0.071429);
		
		for(n=6; n<=24; n++)
		{
			if(n!=11)
			{
				Channelp[n] = Complex_add(num_21, Complex_mul_const(dis_a, (n-11.0)));
				Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
				Output[n_out++] = Complex_div(number[n], Channelf[n]);
			}
		}
		
		
		for(n=26; n<=38; n++)
		{
			if(n!=32)
			{
				Channelp[n] = Complex_add(num_7, Complex_mul_const(dis_b, (n-25.0)));
				Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
				Output[n_out++] = Complex_div(number[n], Channelf[n]);
			}
		}
		
		for(n=40; n<=58; n++)
		{
			if(n!=53)
			{
				Channelp[n] = Complex_add(num7, Complex_mul_const(dis_c, (n-39.0)));
				Channelf[n] = Complex_add(Complex_mul_const(Channel[n], Xa), Complex_mul_const(Channelp[n], Xb));
				Output[n_out++] = Complex_div(number[n], Channelf[n]);
			}
		}
	
	}
	
	return 1;
}






