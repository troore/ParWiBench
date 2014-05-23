

//return freq_offset and count result before SIGNAL part!! 
float Get_freq_offset(Complex *Input, Complex *Output)
{
	int i;
	float sum_angle;
	float freq_offset; 
	
	sum_angle = 0.0;
	
	for(i=0; i<64; i++)
	{
		sum_angle += Complex_angle(Complex_div(Input[i+16], Input[i]))/16.0;
	}
	
	freq_offset = sum_angle / 64.0;
	
	for(i=0; i<240; i++)
	{
		Output[i] = Complex_mul(Input[i+80], Complex_expi(0.0-freq_offset*i) );
	} 
	
	return freq_offset;
}


//deal with the data part!
int Freq_synchro(Complex *Input, Complex *Output, int len_symbol, float freq_offset)
{
	int i;
	for(i=0; i<(len_symbol-1)*80; i++)
	{
		Output[i] = Complex_mul(Input[i], Complex_expi(0.0-freq_offset*i) );
	}
	return 1;
}




