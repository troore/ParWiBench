int De_interleaver_s(char *Input, char *Output)
{
	
	int i, j, k, m, n;
	int n_in, n_out;
	char inputs[288], outputs[288];
	
	n_in = 0;
	n_out = 0;
	
	
	//First deal with SIGNAL part!
	//store the bits
	for(j=0; j<6; j++)
	{
		for(k=0; k<8; k++)
		{
			inputs[8*j+k]=((Input[n_in])>>k) & 0x01;
		}
		n_in++;
	}
	
	//map
	for(j=0; j<48; j++)
	{
		m = 16*j-47*(16*j/48);
		outputs[m] = inputs[j];
	}
	
	//outputs the bits
	for(j=0; j<6; j++)
	{
		Output[n_out] = 0;
		for(k=0; k<8; k++)
		{
			Output[n_out] = Output[n_out] | (outputs[8*j+k]<<k);
		}
		n_out++;
	}
	
	return 1;
	
}


int De_interleaver_d(char *Input, char *Output, int mod_type, int len_afterbcc)
{
	int i, j, k, m, n;
	int n_in, n_out;
	char inputs[288], outputs[288];
	
	n_in = 0;
	n_out = 0;
	
	
	//BPSK type: Ncbps=48, Nbpsc=1
	//input:6 byte
	//output:6 byte
	if(mod_type == BPSK)
	{
		for(i=0; i<len_afterbcc/6; i++)
		{
			//store the bits
			for(j=0; j<6; j++)
			{
				for(k=0; k<8; k++)
				{
					inputs[8*j+k]=((Input[n_in])>>k) & 0x01;
				}
				n_in++;
			}
			
			//map
			for(j=0; j<48; j++)
			{
				m = 16*j-47*(16*j/48);
				outputs[m] = inputs[j];
			}
			
			//outputs the bits
			for(j=0; j<6; j++)
			{
				Output[n_out] = 0;
				for(k=0; k<8; k++)
				{
					Output[n_out] = Output[n_out] | (outputs[8*j+k]<<k);
				}
				n_out++;
			}
		}
	}
	
	
	
	//QPSK type: Ncbps=96, Nbpsc=2
	//input:12 byte
	//output:12 byte
	else if(mod_type == QPSK)
	{
		for(i=0; i<len_afterbcc/12; i++)
		{
			//store the inputs
			for(j=0; j<12; j++)
			{
				for(k=0; k<8; k++)
				{
					inputs[8*j+k] = ((Input[n_in])>>k) & 0x01;
				}
				n_in++;
			}
			
			//map
			for(j=0; j<96; j++)
			{
				m=16*j-95*(16*j/96);
				outputs[m] = inputs[j];
			}
			
			//output the result!
			for(j=0; j<12; j++)
			{
				Output[n_out] = 0;
				for(k=0; k<8; k++)
				{
					Output[n_out] = Output[n_out] | (outputs[8*j+k]<<k);
				}
				n_out++;
			}
			
		}
	}
	
	
	
	//QAM16 type: Ncbps=192, Nbpsc=4
	//input:24 byte
	//output:24 byte
	else if(mod_type == QAM16)
	{
		for(i=0; i<len_afterbcc/24; i++)
		{
			//store the inputs
			for(j=0; j<24; j++)
			{
				for(k=0; k<8; k++)
				{
					inputs[8*j+k]=((Input[n_in])>>k) & 0x01;
				}
				n_in++;
			}
			
			//map
			for(j=0; j<192; j++)
			{
				m=2*(j/2)+(j+(16*j/192))%2;
				n=16*m-(191)*(16*m/192);
				outputs[n]=inputs[j];
			}
			
			//output result
			for(j=0; j<24; j++)
			{
				Output[n_out] = 0;
				for(k=0; k<8; k++)
				{
					Output[n_out] = Output[n_out] | (outputs[8*j+k]<<k);
				}
				n_out++;
			}
			
		}
	}
	
	
	//QAM64 type: Ncbps=288, Nbpsc=6
	//input:36 byte
	//output:36 byte
	else if(mod_type == QAM64)
	{
		for(i=0; i<len_afterbcc/36; i++)
		{
			//store the inputs
			for(j=0; j<36; j++)
			{
				for(k=0; k<8; k++)
				{
					inputs[8*j+k]=((Input[n_in])>>k) & 0x01;
				}
				n_in++;
			}
			
			//map
			for(j=0; j<288; j++)
			{
				m=3*(j/3)+(j+(16*j/288))%3;
				n=16*m-(287)*(16*m/288);
				outputs[n]=inputs[j];
			}
			
			//output result
			for(j=0; j<36; j++)
			{
				Output[n_out] = 0;
				for(k=0; k<8; k++)
				{
					Output[n_out] = Output[n_out] | (outputs[8*j+k]<<k);
				}
				n_out++;
			}	
			
			
		}
	}
	
	
	return 1;
}







