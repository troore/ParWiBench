#include <stdio.h>

#include "../PHY_TX.h"

char input_buffer[8000];
Complex output_buffer[240000];

char Output_scrambler[8000];
char Output_bcc[16000];
char Output_interleaver[16000];
Complex Output_constellation_map[128000];
Complex Output_insert_pilot[180000];
Complex Output_ifft[180000];
Complex Output_insert_gi[232000];
Complex Output_add_preamble[240000];

int PHY_SW_TX(char *input_buffer, Complex *output_buffer, int datarate, int length)
{
	//int datarate;
	int code_rate;
	int mod_type;
	int bcc_rate;
	int Ndbps;
	
	int len_psdu;       //length of PSDU (byte)
	int len_prebcc;     //length without SIGNAL part before BCC (byte)
	int len_afterbcc;   //length without SIGNAL part after BCC (byte)
	int len_tail;       //length of Tail part including extra 22bits (byte)
	int len_symbol;     //length of symbol

	//Print_char_UsrHex(input_buffer, 111);
	//Print_char_StrHex(input_buffer, 111);

	len_psdu = length;
	Init_para_n(input_buffer, &datarate, &code_rate, &mod_type, &bcc_rate, &Ndbps, &len_psdu, &len_prebcc, &len_afterbcc, &len_tail, &len_symbol);
	//Print_char_UsrHex(input_buffer, len_prebcc+3);

	Scrambler(input_buffer, Output_scrambler, len_prebcc, len_tail);
	//Print_char_StrHex(Output_scrambler, 3+108);

	BCC(Output_scrambler, Output_bcc, len_prebcc, bcc_rate);
	//Print_char_StrHex(Output_bcc, 6+144);

	Interleaver(Output_bcc, Output_interleaver, mod_type, len_afterbcc);
	//Print_char_StrHex(Output_interleaver, 6+144);

	Constellation_map(Output_interleaver, Output_constellation_map, mod_type, len_afterbcc);
	//Print_complex_Str(Output_constellation_map, 48+144*8/4);

	Insert_pilot(Output_constellation_map, Output_insert_pilot, len_symbol);
	//Print_complex_Str(Output_insert_pilot, 128);

	IFFT(Output_insert_pilot, Output_ifft, len_symbol);
	//Print_complex_Str(Output_ifft, 64+10);

	Insert_GI(Output_ifft, Output_insert_gi, len_symbol);
	//Print_complex_Str(Output_insert_gi, len_symbol*80+10);

	Add_preamble(Output_insert_gi, output_buffer, len_symbol);
	//Print_complex_Str(Output_add_preamble, 320+len_symbol*80+1);

	return 320+len_symbol*80+1;
}




int main()
{	
	char addr[30] = "input.txt";
	Read_input(input_buffer, addr);
	
	int Output_num;	

	Output_num = PHY_SW_TX(input_buffer, output_buffer, 6, 100);

	Print_complex_Str(output_buffer, Output_num);

	return 0;
}



