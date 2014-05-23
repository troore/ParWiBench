int Insert_GI(Complex *Input, Complex *Output, int len_symbol)
{
	Complex former;
	//<-----------Something wrong with here!-----------------> 
	former = Complex_init(0.156, 0.0);
	
	int i, n;
	int n_in, n_out;
	n_in = 0;
	n_out = 0;
	
	Complex data_cplx[64];
	for(n=0; n<len_symbol; n++)
	{
		for(i=0; i<64; i++)
		{
			data_cplx[i] = Input[n_in++]; 
		}
		
		Output[n_out++] = Complex_init((data_cplx[48].real + former.real)/2 , (data_cplx[48].imag + former.imag)/2);
		
		for(i=1; i<16; i++)
		{
			Output[n_out++] = data_cplx[i+48];
		}
		for(i=0; i<64; i++)
		{
			Output[n_out++] = data_cplx[i];
		}
		
		former = data_cplx[0];
	} 
	
	Output[n_out++] = Complex_init(data_cplx[0].real/2, data_cplx[0].imag/2);
	former = data_cplx[16];
	 
	
	return 1; 
	 
} 
