int Constellation_demap_s(Complex *Input, char *Output)
{
	int i, j;
	char out_temp;
	char data[24];
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	for(i=0; i<6; i++)
	{
		for(j=0; j<8; j++)
		{
			if(Input[n_in++].real > 0)
				data[j] = 1;
			else
				data[j] = 0;
		}
		
		out_temp = 0;
		
		for(j=0; j<8; j++)
		{
			out_temp = out_temp | (data[j]<<j);
		}
		
		Output[n_out++] = out_temp;
		
	}
	
	return 1;
}



int Constellation_demap_d(Complex *Input, char *Output, int mod_type, int len_symbol)
{
	int i, j;
	char in_temp;
	char out_temp;
	char data[24];
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	
	if(mod_type == BPSK)
	{
		for(i=0; i<(len_symbol-1)*6; i++)
		{
			for(j=0; j<8; j++)
			{
				if(Input[n_in++].real > 0)
					data[j] = 1;
				else
					data[j] = 0;
			}
			
			out_temp = 0;
			
			for(j=0; j<8; j++)
			{
				out_temp = out_temp | (data[j]<<j);
			}
			
			Output[n_out++] = out_temp;
			
		}
	}
	
	else if(mod_type == QPSK)
	{
		for(i=0; i<(len_symbol-1)*12; i++)
		{
			for(j=0; j<8; j+=2)
			{
				if(Input[n_in].real > 0)
					data[j] = 1;
				else
					data[j] = 0;
					
				if(Input[n_in++].imag > 0)
					data[j+1] = 1;
				else
					data[j+1] = 0;
			}
			
			out_temp = 0;
			
			for(j=0; j<8; j++)
			{
				out_temp = out_temp | (data[j]<<j);
			}
			
			Output[n_out++] = out_temp;
		}
	}
	
	
	else if(mod_type == QAM16)
	{
		for(i=0; i<(len_symbol-1)*24; i++)
		{
			for(j=0; j<8; j+=4)
			{
				if((Input[n_in].real)<-0.642455) {data[j]=0;data[j+1]=0;}
				else if((Input[n_in].real)<0.0) {data[j]=0;data[j+1]=1;}
				else if((Input[n_in].real)<0.642455) {data[j]=1;data[j+1]=1;}
				else {data[j]=1;data[j+1]=0;}
				
				if((Input[n_in].imag)<-0.642455) {data[j+2]=0;data[j+3]=0;}
				else if((Input[n_in].imag)<0.0) {data[j+2]=0;data[j+3]=1;}
				else if((Input[n_in].imag)<0.642455) {data[j+2]=1;data[j+3]=1;}
				else {data[j+2]=1;data[j+3]=0;}
				
				n_in++;
			}
			
			out_temp = 0;
			
			for(j=0; j<8; j++)
			{
				out_temp = out_temp | (data[j]<<j);
			}
			
			Output[n_out++] = out_temp;
		}
	}
	
	
	else if(mod_type == QAM64)
	{
		for(i=0; i<(len_symbol-1)*12; i++)
		{
			if((Input[n_in].real)<-0.92582) {data[0]=0;data[1]=0;data[2]=0;}
			else if((Input[n_in].real)<-0.617214) {data[0]=0;data[1]=0;data[2]=1;}
			else if((Input[n_in].real)<-0.308607) {data[0]=0;data[1]=1;data[2]=1;}
			else if((Input[n_in].real)<0.0) {data[0]=0;data[1]=1;data[2]=0;}
			else if((Input[n_in].real)<0.308607) {data[0]=1;data[1]=1;data[2]=0;}
			else if((Input[n_in].real)<0.617214) {data[0]=1;data[1]=1;data[2]=1;}
			else if((Input[n_in].real)<0.92582) {data[0]=1;data[1]=0;data[2]=1;}
			else {data[0]=1;data[1]=0;data[2]=0;}
			
			if((Input[n_in].imag)<-0.92582) {data[3]=0;data[4]=0;data[5]=0;}
			else if((Input[n_in].imag)<-0.617214) {data[3]=0;data[4]=0;data[5]=1;}
			else if((Input[n_in].imag)<-0.308607) {data[3]=0;data[4]=1;data[5]=1;}
			else if((Input[n_in].imag)<0.0) {data[3]=0;data[4]=1;data[5]=0;}
			else if((Input[n_in].imag)<0.308607) {data[3]=1;data[4]=1;data[5]=0;}
			else if((Input[n_in].imag)<0.617214) {data[3]=1;data[4]=1;data[5]=1;}
			else if((Input[n_in].imag)<0.92582) {data[3]=1;data[4]=0;data[5]=1;}
			else {data[3]=1;data[4]=0;data[5]=0;}
			
			n_in++;
			
			
			
			
			if((Input[n_in].real)<-0.92582) {data[6]=0;data[7]=0;data[8]=0;}
			else if((Input[n_in].real)<-0.617214) {data[6]=0;data[7]=0;data[8]=1;}
			else if((Input[n_in].real)<-0.308607) {data[6]=0;data[7]=1;data[8]=1;}
			else if((Input[n_in].real)<0.0) {data[6]=0;data[7]=1;data[8]=0;}
			else if((Input[n_in].real)<0.308607) {data[6]=1;data[7]=1;data[8]=0;}
			else if((Input[n_in].real)<0.617214) {data[6]=1;data[7]=1;data[8]=1;}
			else if((Input[n_in].real)<0.92582) {data[6]=1;data[7]=0;data[8]=1;}
			else {data[6]=1;data[7]=0;data[8]=0;}
			
			if((Input[n_in].imag)<-0.92582) {data[9]=0;data[10]=0;data[11]=0;}
			else if((Input[n_in].imag)<-0.617214) {data[9]=0;data[10]=0;data[11]=1;}
			else if((Input[n_in].imag)<-0.308607) {data[9]=0;data[10]=1;data[11]=1;}
			else if((Input[n_in].imag)<0.0) {data[9]=0;data[10]=1;data[11]=0;}
			else if((Input[n_in].imag)<0.308607) {data[9]=1;data[10]=1;data[11]=0;}
			else if((Input[n_in].imag)<0.617214) {data[9]=1;data[10]=1;data[11]=1;}
			else if((Input[n_in].imag)<0.92582) {data[9]=1;data[10]=0;data[11]=1;}
			else {data[9]=1;data[10]=0;data[11]=0;}
			
			n_in++;
			
			
			
			if((Input[n_in].real)<-0.92582) {data[12]=0;data[13]=0;data[14]=0;}
			else if((Input[n_in].real)<-0.617214) {data[12]=0;data[13]=0;data[14]=1;}
			else if((Input[n_in].real)<-0.308607) {data[12]=0;data[13]=1;data[14]=1;}
			else if((Input[n_in].real)<0.0) {data[12]=0;data[13]=1;data[14]=0;}
			else if((Input[n_in].real)<0.308607) {data[12]=1;data[13]=1;data[14]=0;}
			else if((Input[n_in].real)<0.617214) {data[12]=1;data[13]=1;data[14]=1;}
			else if((Input[n_in].real)<0.92582) {data[12]=1;data[13]=0;data[14]=1;}
			else {data[12]=1;data[13]=0;data[14]=0;}
			
			if((Input[n_in].imag)<-0.92582) {data[15]=0;data[16]=0;data[17]=0;}
			else if((Input[n_in].imag)<-0.617214) {data[15]=0;data[16]=0;data[17]=1;}
			else if((Input[n_in].imag)<-0.308607) {data[15]=0;data[16]=1;data[17]=1;}
			else if((Input[n_in].imag)<0.0) {data[15]=0;data[16]=1;data[17]=0;}
			else if((Input[n_in].imag)<0.308607) {data[15]=1;data[16]=1;data[17]=0;}
			else if((Input[n_in].imag)<0.617214) {data[15]=1;data[16]=1;data[17]=1;}
			else if((Input[n_in].imag)<0.92582) {data[15]=1;data[16]=0;data[17]=1;}
			else {data[15]=1;data[16]=0;data[17]=0;}
			
			n_in++;
			
			
			
			if((Input[n_in].real)<-0.92582) {data[18]=0;data[19]=0;data[20]=0;}
			else if((Input[n_in].real)<-0.617214) {data[18]=0;data[19]=0;data[20]=1;}
			else if((Input[n_in].real)<-0.308607) {data[18]=0;data[19]=1;data[20]=1;}
			else if((Input[n_in].real)<0.0) {data[18]=0;data[19]=1;data[20]=0;}
			else if((Input[n_in].real)<0.308607) {data[18]=1;data[19]=1;data[20]=0;}
			else if((Input[n_in].real)<0.617214) {data[18]=1;data[19]=1;data[20]=1;}
			else if((Input[n_in].real)<0.92582) {data[18]=1;data[19]=0;data[20]=1;}
			else {data[18]=1;data[19]=0;data[20]=0;}
			
			if((Input[n_in].imag)<-0.92582) {data[21]=0;data[22]=0;data[23]=0;}
			else if((Input[n_in].imag)<-0.617214) {data[21]=0;data[22]=0;data[23]=1;}
			else if((Input[n_in].imag)<-0.308607) {data[21]=0;data[22]=1;data[23]=1;}
			else if((Input[n_in].imag)<0.0) {data[21]=0;data[22]=1;data[23]=0;}
			else if((Input[n_in].imag)<0.308607) {data[21]=1;data[22]=1;data[23]=0;}
			else if((Input[n_in].imag)<0.617214) {data[21]=1;data[22]=1;data[23]=1;}
			else if((Input[n_in].imag)<0.92582) {data[21]=1;data[22]=0;data[23]=1;}
			else {data[21]=1;data[22]=0;data[23]=0;}
			
			n_in++;
			
			out_temp=0;
			for(j=0;j<8;j++){
				out_temp = out_temp | (data[j]<<j);
			}
			Output[n_out++]=out_temp;

			out_temp=0;
			for(j=0;j<8;j++){
				out_temp = out_temp | (data[j+8]<<j);
			}
			Output[n_out++]=out_temp;

			out_temp=0;
			for(j=0;j<8;j++){
				out_temp = out_temp | (data[j+16]<<j);
			}
			Output[n_out++]=out_temp;
		}
	}
	
	return 1;
}



