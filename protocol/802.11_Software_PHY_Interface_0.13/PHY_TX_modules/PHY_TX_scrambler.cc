int Scrambler(char *Input, char *Output, int len_prebcc, int len_tail)
{
	int i, j, k;  //variable use for count.
	char byte_out, bit_in, bit_out;   //temp variable
	
	int random[8];
	random[7] = 1;
	random[6] = 0;
	random[5] = 1;
	random[4] = 1;
	random[3] = 1;
	random[2] = 0;
	random[1] = 1;
	random[0] = 0;	
	
	//Deal with the SIGNAL part (Do not need to scramble)
	Output[0] = Input[0];
	Output[1] = Input[1];
	Output[2] = Input[2];

	//scrambler start.
	for(i=3; i<3+len_prebcc; i++)
	{
		byte_out = 0;
		for(k=0; k<=7; k++)
		{
			bit_in = (Input[i] >> k) &0x01;
			bit_out = (random[7] ^ random[4] ^ bit_in) & 0x01;
			random[0] = random[7] ^ random[4];
			for(j=7; j>0; j--)
			{
				random[j] = random[j-1];
			}
			byte_out |= (bit_out << k); 
		}
		Output[i] = byte_out;
	}

	//first 6bit of Tail part need to set to 0.
	Output[3+len_prebcc-len_tail] &= 0xc0;
	
	return 1;
}



