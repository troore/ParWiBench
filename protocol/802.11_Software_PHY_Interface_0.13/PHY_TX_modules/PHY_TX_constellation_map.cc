int Constellation_map(char *Input, Complex *Output, int mod_type, int len_afterbcc)
{
	int i,j,k;
	int ti,tq;
	int n_in, n_out;
	float temp[8];
	char data[24];
	
	n_in = 0;
	n_out = 0;
	
	//Deal with the SIGNAL part!
	for(i=0; i<6; i++)
	{
		for(j=0; j<8; j++)
		{
			Output[n_out++] = (((1<<j)&(Input[n_in])) == 0) ? Complex_init(-1.0, 0.0) : Complex_init(1.0, 0.0);
		}
		n_in++;
	}
	
	
	
	
	//BPSK
	if(mod_type == BPSK)
	{
		for(i=0; i<len_afterbcc; i++)
		{
			for(j=0; j<8; j++)
			{
				Output[n_out++] = (((1<<j)&(Input[n_in])) == 0) ? Complex_init(-1.0, 0.0) : Complex_init(1.0, 0.0);
			}
			n_in++;	
		}
	}
	
	
	//QPSK
	else if(mod_type == QPSK)
	{
		temp[0] = -0.707107;
		temp[1] = 0.707107;
		
		for(i=0; i<len_afterbcc; i++)
		{
			for(j=0; j<8; j+=2)
			{
				Output[n_out++] = Complex_init(temp[0x01&((Input[n_in])>>j)], temp[0x01&((Input[n_in])>>(j+1))]);
			}
			n_in++;
		}
	}
	
	
	//QAM16
	else if(mod_type == QAM16)
	{
		temp[0]=-0.948683;
		temp[1]=0.948683;
		temp[2]=-0.316228;
		temp[3]=0.316228;
		
		for(i=0; i<len_afterbcc; i++)
		{
			for(j=0; j<8; j+=4)
			{
				Output[n_out++] = Complex_init(temp[0x03&((Input[n_in])>>j)], temp[0x03&((Input[n_in])>>(j+2))]);
			}
			n_in++;
		}
	}
	
	
	//QAM64
	else if(mod_type == QAM64)
	{
		temp[0]=-1.080123;
		temp[1]=1.080123;
		temp[2]=-0.154303;
		temp[3]=0.154303;;
		temp[4]=-0.771517;
		temp[5]=0.771517;
		temp[6]=-0.462910;
		temp[7]=0.462910;
		
		for(i=0; i<len_afterbcc/3; i++)
		{
			for(j=0; j<3; j++)
			{
				for(k=0; k<8; k++)
				{
					data[j*8+k]=(((1<<k) & (Input[n_in])) == 0)?0:1;
				}
				n_in++;
			}
			
			for(j=0; j<4; j++)
			{
				ti = data[j*6] | (data[j*6+1]<<1) | (data[j*6+2]<<2);
				tq = data[j*6+3] | (data[j*6+4]<<1) | (data[j*6+5]<<2);
				Output[n_out++] = Complex_init(temp[ti], temp[tq]);
			}
		}
	}
	
	
	return 1;
}






