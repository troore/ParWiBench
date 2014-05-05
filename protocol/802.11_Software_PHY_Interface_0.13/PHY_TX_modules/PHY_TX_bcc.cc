int BCC(char *Input, char *Output, int len_prebcc, int bcc_rate)
{
	int i, j, n, k;
	int n_in, n_out;
	int out_a, out_b, out_x;
	
	int shift[8];  //shift reg, use for BCC coding
	shift[0] = 0;
	shift[1] = 0;
	shift[2] = 0;
	shift[3] = 0;
	shift[4] = 0;
	shift[5] = 0;
	shift[6] = 0;
	shift[7] = 0;
	
	n_in = 0;
	n_out = 0;
	
	//Deal with the SIGNAL part.
	for(i=0; i<3; i++)
	{
		out_a = 0;
		out_b = 0;
		out_x = 0;
		
		for(j=0; j<8; j++)
		{
			shift[7] = shift[6];
			shift[6] = shift[5];
			shift[5] = shift[4];
			shift[4] = shift[3];
			shift[3] = shift[2];
			shift[2] = shift[1];
			shift[1] = shift[0];
			shift[0] = (Input[n_in]>>j)&0x01;
			
			out_a = (shift[0]+shift[2]+shift[3]+shift[5]+shift[6])%2;
			out_b = (shift[0]+shift[1]+shift[2]+shift[3]+shift[6])%2;
			out_x = out_x | (out_a<<((2*j)%8)) | (out_b<<((2*j+1)%8));
			
			if(j==3)
			{
				Output[n_out++] = out_x;
				out_x = 0;
			}
		}	
		
		Output[n_out++] = out_x;
		n_in++;	
	}
	
	
	
	
	
	if(bcc_rate == R_1_2)
	{
		for(i=0; i<len_prebcc; i++)
		{
			out_a = 0;
			out_b = 0;
			out_x = 0;
			
			for(j=0; j<8; j++)
			{
				shift[7] = shift[6];
				shift[6] = shift[5];
				shift[5] = shift[4];
				shift[4] = shift[3];
				shift[3] = shift[2];
				shift[2] = shift[1];
				shift[1] = shift[0];
				shift[0] = (Input[n_in]>>j)&0x01;
				
				out_a = (shift[0]+shift[2]+shift[3]+shift[5]+shift[6])%2;
				out_b = (shift[0]+shift[1]+shift[2]+shift[3]+shift[6])%2;
				out_x = out_x | (out_a<<((2*j)%8)) | (out_b<<((2*j+1)%8));
				
				if(j==3)
				{
					Output[n_out++] = out_x;
					out_x = 0;
				}
			}	
			
			Output[n_out++] = out_x;
			n_in++;	
		}
	}
	
	else if(bcc_rate == R_2_3)
	{
		for(i=0; i<len_prebcc/2; i++)
		{
			k = 0;
			out_x = 0;
			
			for(n=0; n<2; n++)
			{
				for(j=0; j<8; j++)
				{
					shift[7] = shift[6];
					shift[6] = shift[5];
					shift[5] = shift[4];
					shift[4] = shift[3];
					shift[3] = shift[2];
					shift[2] = shift[1];
					shift[1] = shift[0];
					shift[0] = (Input[n_in]>>j)&0x01;
					
					out_a = (shift[0]+shift[2]+shift[3]+shift[5]+shift[6])%2;
					out_x = out_x | (out_a<<k);
					
					k = (k+1) % 8;
					if(k == 0)
					{
						Output[n_out++] = out_x;
						out_x = 0;
					}
					
					out_b = (shift[0]+shift[1]+shift[2]+shift[3]+shift[6])%2;
					
					if(j%2 == 0)
					{
						out_x = out_x | (out_b << k);
						k = (k+1) % 8;
						if(k == 0)
						{
							Output[n_out++] = out_x;
							out_x = 0;
						}
					}
				}
				n_in++;
			}
		}
	}
	
	else if(bcc_rate == R_3_4)
	{
		for(i=0; i<len_prebcc/3; i++)
		{
			k = 0;
			out_x = 0;
			
			for(n=0; n<3; n++)
			{
				for(j=0; j<8; j++)
				{
					shift[7] = shift[6];
					shift[6] = shift[5];
					shift[5] = shift[4];
					shift[4] = shift[3];
					shift[3] = shift[2];
					shift[2] = shift[1];
					shift[1] = shift[0];
					shift[0] = (Input[n_in]>>j)&0x01;
					
					out_a = (shift[0]+shift[2]+shift[3]+shift[5]+shift[6])%2;
					
					if(((n*8+j)%3)!=2)
					{
						out_x = out_x | (out_a<<k);
						k = (k+1) % 8;
						if(k == 0)
						{
							(Output[n_out++] = out_x);
							out_x = 0;
						}
					}
					out_b = (shift[0]+shift[1]+shift[2]+shift[3]+shift[6])%2;
					
					if(((n*8+j)%3)!=1)
					{
						out_x = out_x | (out_b<<k);
						k = (k+1) % 8;
						if(k==0)
						{
							(Output[n_out++] = out_x);
							out_x = 0;
						}	
					}
				}
				n_in++;
			}
		}
	}
	
	
		
	return 1;	
}

	
