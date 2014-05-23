int Add_preamble(Complex *Input, Complex *Output, int len_symbol)
{
	
	int i,j;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	
	Complex Short_trs[16];
	Complex Long_trs[64];
	

	
	Short_trs[0]=Complex_init(0.046, 0.046);
	Short_trs[1]=Complex_init(-0.132, 0.002);
	Short_trs[2]=Complex_init(-0.013, -0.079);
	Short_trs[3]=Complex_init(0.143, -0.013);
	Short_trs[4]=Complex_init(0.092, 0.000);
	Short_trs[5]=Complex_init(0.143, -0.013);
	Short_trs[6]=Complex_init(-0.013, -0.079);
	Short_trs[7]=Complex_init(-0.132, 0.002);

	Short_trs[8]=Complex_init(0.046, 0.046);
	Short_trs[9]=Complex_init(0.002, -0.132);
	Short_trs[10]=Complex_init(-0.079, -0.013);
	Short_trs[11]=Complex_init(-0.013, 0.143);
	Short_trs[12]=Complex_init(0.000, 0.092);
	Short_trs[13]=Complex_init(-0.013, 0.143);
	Short_trs[14]=Complex_init(-0.079, -0.013);
	Short_trs[15]=Complex_init(0.002, -0.132);



	Long_trs[0]=Complex_init(0.156, 0.000);
	Long_trs[1]=Complex_init(-0.005, -0.120);
	Long_trs[2]=Complex_init(0.040, -0.111);
	Long_trs[3]=Complex_init(0.097, 0.083);
	Long_trs[4]=Complex_init(0.021, 0.028);
	Long_trs[5]=Complex_init(0.060, -0.088);
	Long_trs[6]=Complex_init(-0.115, -0.055);
	Long_trs[7]=Complex_init(-0.038, -0.106);

	Long_trs[8]=Complex_init(0.098, -0.026);
	Long_trs[9]=Complex_init(0.053, 0.004);
	Long_trs[10]=Complex_init(0.001, -0.115);
	Long_trs[11]=Complex_init(-0.137, -0.047);
	Long_trs[12]=Complex_init(0.024, -0.059);
	Long_trs[13]=Complex_init(0.059, -0.015);
	Long_trs[14]=Complex_init(-0.022, 0.161);
	Long_trs[15]=Complex_init(0.119, -0.004);

	Long_trs[16]=Complex_init(0.062, -0.062);
	Long_trs[17]=Complex_init(0.037, 0.098);
	Long_trs[18]=Complex_init(-0.057, 0.039);
	Long_trs[19]=Complex_init(-0.131, 0.065);
	Long_trs[20]=Complex_init(0.082, 0.092);
	Long_trs[21]=Complex_init(0.070, 0.014);
	Long_trs[22]=Complex_init(-0.060, 0.081);
	Long_trs[23]=Complex_init(-0.056, -0.022);

	Long_trs[24]=Complex_init(-0.035, -0.151);
	Long_trs[25]=Complex_init(-0.122, -0.017);
	Long_trs[26]=Complex_init(-0.127, -0.021);
	Long_trs[27]=Complex_init(0.075, -0.074);
	Long_trs[28]=Complex_init(-0.003, 0.054);
	Long_trs[29]=Complex_init(-0.092, 0.115);
	Long_trs[30]=Complex_init(0.092, 0.106);
	Long_trs[31]=Complex_init(0.012, 0.098);

	Long_trs[32]=Complex_init(-0.156, 0.000);
	Long_trs[33]=Complex_init(0.012, -0.098);
	Long_trs[34]=Complex_init(0.092, -0.106);
	Long_trs[35]=Complex_init(-0.092, -0.115);
	Long_trs[36]=Complex_init(-0.003, -0.054);
	Long_trs[37]=Complex_init(0.075, 0.074);
	Long_trs[38]=Complex_init(-0.127, 0.021);
	Long_trs[39]=Complex_init(-0.122, 0.017);

	Long_trs[40]=Complex_init(-0.035, 0.151);
	Long_trs[41]=Complex_init(-0.056, 0.022);
	Long_trs[42]=Complex_init(-0.060, -0.081);
	Long_trs[43]=Complex_init(0.070, -0.014);
	Long_trs[44]=Complex_init(0.082, -0.092);
	Long_trs[45]=Complex_init(-0.131, -0.065);
	Long_trs[46]=Complex_init(-0.057, -0.039);
	Long_trs[47]=Complex_init(0.037, -0.098);

	Long_trs[48]=Complex_init(0.062, 0.062);
	Long_trs[49]=Complex_init(0.119, 0.004);
	Long_trs[50]=Complex_init(-0.022, -0.161);
	Long_trs[51]=Complex_init(0.059, 0.015);
	Long_trs[52]=Complex_init(0.024, 0.059);
	Long_trs[53]=Complex_init(-0.137, 0.047);
	Long_trs[54]=Complex_init(0.001, 0.115);
	Long_trs[55]=Complex_init(0.053, -0.004);

	Long_trs[56]=Complex_init(0.098, 0.026);
	Long_trs[57]=Complex_init(-0.038, 0.106);
	Long_trs[58]=Complex_init(-0.115, 0.055);
	Long_trs[59]=Complex_init(0.060, 0.088);
	Long_trs[60]=Complex_init(0.021, -0.028);
	Long_trs[61]=Complex_init(0.097, -0.083);
	Long_trs[62]=Complex_init(0.040, 0.111);
	Long_trs[63]=Complex_init(-0.005, 0.120);
	
	
	//add short traning
	for(i=0; i<10; i++)
	{
		for(j=0; j<16; j++)
		{
			if(i==0 && j==0)
				Output[n_out++] = Complex_init(0.23, 0.23);
			else
				Output[n_out++] = Short_trs[j];
		}
	}
	
	
	//add long traning
	Output[n_out++] = Complex_init(-0.055, 0.023);
	
	for(i=33; i<64; i++)
	{
		Output[n_out++] = Long_trs[i];
	}
	
	for(i=0; i<64; i++)
	{
		Output[n_out++] = Long_trs[i];
	}
		
	for(i=0; i<64; i++)
	{
		Output[n_out++] = Long_trs[i];
	}
	
	
	for(i=0; i<len_symbol*80+1; i++)
	{
		Output[n_out++] = Input[n_in++];
	}
	
	
	
	
	
	return 1;
	
	
}




