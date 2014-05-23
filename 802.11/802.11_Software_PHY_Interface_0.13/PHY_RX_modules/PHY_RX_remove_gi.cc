//Remove GI module

int Remove_GI_s(Complex *Input, Complex *Output)
{
	int i;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	n_in +=32;
	for(i=0; i<128; i++)
	{
		Output[n_out++] = Input[n_in++];
	}

	n_in +=16;	
	for(i=0; i<64; i++)
	{
		Output[n_out++] = Input[n_in++];
	}
	
	return 1;
}


int Remove_GI_d(Complex *Input, Complex *Output, int len_symbol)
{
	int n, i;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	
	for(n=0; n<len_symbol-1; n++)
	{
		n_in +=16;
		for(i=0; i<64; i++)
		{
			Output[n_out++] = Input[n_in++];
		}
	}
	
	
	
	return 1;
}




