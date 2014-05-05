


int Time_synchro(Complex input, Complex *Output, int num_th, int m_th, int mf_th)
{
	int i, n;
	int n_out;
	
	Complex c, d;
	float p, m, mf;
	
	float f_mth;
	float f_mfth;
	
	static int count = 0;
	static int syc = 0;
	static int state = 0;
	static int p_reg = 0;
	
	n_out = 0;

	
	static Complex Shift[32] = {
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0)
								};
								
	static Complex Short_trs[16] = {
									Complex_init(0.002, -0.132),
									Complex_init(-0.079, -0.013),
									Complex_init(-0.013, 0.143),
									Complex_init(0.000, 0.092),
									Complex_init(-0.013, 0.143),
									Complex_init(-0.079, -0.013),
									Complex_init(0.002, -0.132),
									Complex_init(0.046, 0.046),
									
									Complex_init(-0.132, 0.002),
									Complex_init(-0.013, -0.079),
									Complex_init(0.143, -0.013),
									Complex_init(0.092, 0.000),
									Complex_init(0.143, -0.013),
									Complex_init(-0.013, -0.079),
									Complex_init(-0.132, 0.002),
									Complex_init(0.046, 0.046)						
									};
	static Complex Reg[96] = {
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0),
								Complex_init(0.0, 0.0)
								};
								
								
	
	f_mth = (float)m_th/1000.0;
	f_mfth = (float)mf_th/1000.0;
	
	
	//Every loop read only one data.
	//Read input and do some shift count!
	for(i=31; i>=1; i--)
	{
		Shift[i] = Shift[i-1];
	}
	Shift[0] = input;
	
	Reg[p_reg] = Shift[0];
	p_reg = (p_reg+1) % 96; //store the datas to the reg.
	
	
	//Start to count the para
	c = Complex_init(0.0, 0.0);
	for(i=0; i<16; i++)
	{
		c = Complex_add(c, Complex_mul_conj(Shift[i], Shift[i+16]));
	}
	
	d = Complex_init(0.0, 0.0);
	for(i=0; i<16; i++)
	{
		d = Complex_add(d, Complex_mul_conj(Short_trs[i], Shift[i+16]));
	}
	
	p = 0.0;
	for(i=16; i<32; i++)
	{
		p = p + Shift[i].real*Shift[i].real + Shift[i].imag*Shift[i].imag;
	}
	
	if(p == 0)
	{
		m = 0.0;
		mf = 0.0;
	}
	else
	{
		m = (c.real*c.real + c.imag*c.imag) / (p*p);
		mf = (d.real*d.real + d.imag*d.imag) * 5.0 / p;
	}
	
	
	//start to do the state control!
	if(state == 0)
	{
		if(m>f_mth)
			count++;
		else
			count = 0;
	
	
		if(count >= num_th)
		{
			state = 1;
			count = 0;
		}
	}
	
	else if(state == 1)
	{
		if(m<f_mth)
			count++;
		if(count>=num_th)
		{
			state = 0;
			count = 0;
		}
		
		if(m<f_mth&&mf>f_mfth)
		{
			state = 0;
			count = 0;
			for(n=0; n<32; n++)
			{
				Shift[n] = Complex_init(0.0, 0.0);
			}
			for(i=0; i<96; i++)
			{
				Output[n_out++] = Reg[p_reg];
				p_reg = (p_reg+1) % 96;
			}
			
			return 1;
		}
	}
	return 0;
}


int Time_synchro_s(Complex *Input, Complex *Output)
{	
	int i;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	for(i=0; i<80+144; i++)
	{
		Output[n_out++] = Input[n_in++];
	}
	
	return 1;
	
}




int Time_synchro_d(Complex *Input, Complex *Output, int len_symbol)
{
	int n_out, n_in;
	n_out = 0;
	n_in = 0;
	
	int i;
	for(i=0; i<(len_symbol-1)*80; i++)
	{
		Output[n_out++] = Input[n_in++];
	}
	return 1;
}






















