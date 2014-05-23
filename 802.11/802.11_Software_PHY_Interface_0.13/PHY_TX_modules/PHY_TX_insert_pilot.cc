int Insert_pilot(Complex *Input, Complex *Output, int len_symbol)
{
	int i, j, n;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	int shift[8];
	shift[7] = 1;
	shift[6] = 1;
	shift[5] = 1;
	shift[4] = 1;
	shift[3] = 1;
	shift[2] = 1;
	shift[1] = 1;
	shift[0] = 0;
	
	
	for(n=0; n<len_symbol; n++)
	{
		shift[0] = (shift[7] + shift[4]) % 2;
		for(j=7; j>0; j--)
		{
			shift[j] = shift[j-1];
		}
		
		for(i=0; i<6; i++)
		{
			Output[n_out++] = Complex_init(0.0, 0.0);
		}
		for(i=-26; i<=26; i++)
		{
			if(shift[0] == 0)
			{
				if(i == -21) Output[n_out++] = Complex_init(1.0, 0.0);
				else if(i == -7) Output[n_out++] = Complex_init(1.0, 0.0);
				else if(i == 0) Output[n_out++] = Complex_init(0.0, 0.0);
				else if(i == 7) Output[n_out++] = Complex_init(1.0, 0.0);
				else if(i == 21) Output[n_out++] = Complex_init(-1.0, 0.0);
				else Output[n_out++] = Input[n_in++];
			}
			else
			{
				if(i == -21) Output[n_out++] = Complex_init(-1.0, 0.0);
				else if(i == -7) Output[n_out++] = Complex_init(-1.0, 0.0);
				else if(i == 0) Output[n_out++] = Complex_init(0.0, 0.0);
				else if(i == 7) Output[n_out++] = Complex_init(-1.0, 0.0);
				else if(i == 21) Output[n_out++] = Complex_init(1.0, 0.0);
				else Output[n_out++] = Input[n_in++];
			}
		}
		for(i=0; i<5; i++)
		{
			Output[n_out++] = Complex_init(0.0, 0.0);
		}
			
	}
	
	return 1; 
	
}




