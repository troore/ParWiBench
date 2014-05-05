int Init_para(char *SIGNAL, int *datarate, int *code_rate, int *mod_type, int *bcc_rate, int *Ndbps, int *len_psdu, int *len_prebcc, int *len_afterbcc, int *len_tail, int *len_symbol)
{

	int signal2, signal1, signal0;
	signal2 = SIGNAL[2]&0x01;
	signal1 = SIGNAL[1];
	signal0 = SIGNAL[0]&0xe0;

	signal2 = signal2 & 0x0ff;
	signal1 = signal1 & 0x0ff;
	signal0 = signal0 & 0x0ff;

	*len_psdu = signal2*2048 + signal1*8 + signal0/32;

	if(*len_psdu >= 2000 || *len_psdu <= 10)
		return -1;
	
	switch(SIGNAL[0]&0x0f)
	{
		case 11: *datarate = BPSK_1_2;  break;
		case 15: *datarate = BPSK_3_4;  break;
		case 10: *datarate = QPSK_1_2;  break;
		case 14: *datarate = QPSK_3_4;  break;
		case  9: *datarate = QAM16_1_2; break;
		case 13: *datarate = QAM16_3_4; break;
		case  8: *datarate = QAM64_2_3; break;
		case 12: *datarate = QAM64_3_4; break;
		default: return -2; break;
	}
	
	
	if(*datarate == BPSK_1_2)
	{
		*Ndbps = 24;
		*code_rate = 6;
		*mod_type = BPSK;
		*bcc_rate = R_1_2;
	}
	else if(*datarate == BPSK_3_4)
	{
		*Ndbps = 36;
		*code_rate = 9;
		*mod_type = BPSK;
		*bcc_rate = R_3_4;
	}
	else if(*datarate == QPSK_1_2)
	{
		*Ndbps = 48;
		*code_rate = 6;
		*mod_type = QPSK;
		*bcc_rate = R_1_2;
	}
	else if(*datarate == QPSK_3_4)
	{
		*Ndbps = 72;
		*code_rate = 9;
		*mod_type = QPSK;
		*bcc_rate = R_3_4;
	}
	else if(*datarate == QAM16_1_2)
	{
		*Ndbps = 96;
		*code_rate = 6;
		*mod_type = QAM16;
		*bcc_rate = R_1_2;
	}
	else if(*datarate == QAM16_3_4)
	{
		*Ndbps = 144;
		*code_rate = 9;
		*mod_type = QAM16;
		*bcc_rate = R_3_4;
	}
	else if(*datarate == QAM64_2_3)
	{
		*Ndbps = 192;
		*code_rate = 8;
		*mod_type = QAM64;
		*bcc_rate = R_2_3;
	}
	else if(*datarate == QAM64_3_4)
	{
		*Ndbps = 216;
		*code_rate = 9;
		*mod_type = QAM64;
		*bcc_rate = R_3_4;
	}
	else
	{
		printf("Error: datarate get a wrong number!\n");
	}
	
	*len_prebcc = *Ndbps * ((*len_psdu*8+22)/ (*Ndbps)+1) / 8;
	*len_afterbcc = *len_prebcc * 12 / *code_rate;
	*len_tail = *len_prebcc - *len_psdu - 2;
	
	if(*mod_type == BPSK) *len_symbol = *len_afterbcc*8/48+1;
	else if(*mod_type == QPSK) *len_symbol = *len_afterbcc*8/96+1;
	else if(*mod_type == QAM16) *len_symbol = *len_afterbcc*8/192+1;
	else if(*mod_type == QAM64) *len_symbol = *len_afterbcc*8/288+1;


	//deal with 4bit wrong problem.
	if(*datarate == BPSK_3_4)
	{

		//*len_symbol = (*len_psdu*8+22)/(*Ndbps) + 1;
printf("%d\n", *len_symbol);
		//if(*len_symbol % 2 == 1)
		if( ((*len_psdu*8+22) / (*Ndbps) + 1) % 2 == 1)
		{
			printf("change\n");
			*len_symbol = (*len_psdu*8+22)/(*Ndbps) + 1+2;
			*len_prebcc = (*len_symbol-1) * (*Ndbps)/8;
			*len_afterbcc = *len_prebcc * 12 / *code_rate;
			*len_tail = *len_prebcc - *len_psdu - 2; 
			
		}
	}

	return 1;
}







