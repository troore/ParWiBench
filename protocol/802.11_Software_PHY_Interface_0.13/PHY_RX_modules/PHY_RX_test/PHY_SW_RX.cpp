#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;
#include "../PHY_RX.h"




int datarate;
int code_rate;
int mod_type;
int bcc_rate;
int Ndbps;

int len_psdu;       //length of PSDU (byte)
int len_prebcc;     //length without SIGNAL part before BCC (byte)
int len_afterbcc;   //length without SIGNAL part after BCC (byte)
int len_tail;       //length of Tail part including extra 22bits (byte)
int len_symbol;     //length of symbol


float freq_offset;
Complex Channel[64];


Complex input_buffer[10000];
char output_buffer[5000];

Complex Output_time_synchro[10000];
Complex Output_freq_synchro[10000];
Complex Output_remove_gi[10000];
Complex Output_fft[10000];
Complex Output_channel_estimation[8000];
char Output_constellation_demap[5000];
char Output_de_interleaver[5000];
char Output_viterbi[5000];
char Output_scrambler[5000];




int PHY_SW_RX(Complex *input_buffer, int length, char *output_buffer)
{

	if(length == 0)
		return 0;



	static int syn_state = 0;
	static int n_signal_output = 0;
	static int n_data_output = 0;

	int n_in;


	n_in = 0;




	while(1)
	{
		if(syn_state == 0)
		{
			while(n_in < length)
			{
				if(Time_synchro(input_buffer[n_in++], Output_time_synchro, 32, 500, 500)==1)
				{
					syn_state = 1;
					n_signal_output = 96;
					break;
				}
			}

			if(n_in >= length)
			{
				return 0;
			}
		}

		else if(syn_state == 1)
		{
			if(n_signal_output + length - n_in < 320)
			{

				while(n_in < length)
				{
					Output_time_synchro[n_signal_output++] = input_buffer[n_in++];
				}
				return 0;
			}

			else
			{
				while(n_signal_output < 320)
				{
					Output_time_synchro[n_signal_output++] = input_buffer[n_in++];
				}
				freq_offset = Get_freq_offset(Output_time_synchro, Output_freq_synchro);	
				Remove_GI_s(Output_freq_synchro, Output_remove_gi);
				FFT_s(Output_remove_gi, Output_fft);
				Channel_estimation_s(Output_fft, Output_channel_estimation, Channel, 0);
				Constellation_demap_s(Output_channel_estimation, Output_constellation_demap);	
				De_interleaver_s(Output_constellation_demap, Output_de_interleaver);
				Viterbi_s(Output_de_interleaver, Output_viterbi);
	
				Init_para(Output_viterbi, &datarate, &code_rate, &mod_type, &bcc_rate, &Ndbps, &len_psdu, &len_prebcc, &len_afterbcc, &len_tail, &len_symbol);
	
//				Print_char_UsrHex(Output_viterbi, 3);
				syn_state = 2;
				n_data_output = 0;

			}
		}

		else if(syn_state == 2)
		{
			if(n_data_output + length - n_in < (len_symbol-1)*80)
			{
				while(n_in < length)
				{
					Output_time_synchro[n_data_output++] = input_buffer[n_in++];
				}
				return 0;
			}

			else
			{
				while(n_data_output < (len_symbol-1)*80)
				{
					Output_time_synchro[n_data_output++] = input_buffer[n_in++];
				}

//				Print_complex_Str(Output_time_synchro, 240);

				Freq_synchro(Output_time_synchro, Output_freq_synchro, len_symbol, freq_offset);
	
				//Print_complex_Str(Output_freq_synchro, (len_symbol-1)*80+240);
	
	
				Remove_GI_d(Output_freq_synchro, Output_remove_gi, len_symbol);
	
				//Print_complex_Str(Output_remove_gi, (len_symbol-1)*64+192);
	
	
				FFT_d(Output_remove_gi, Output_fft, len_symbol);
	
				//Print_complex_Str(Output_fft, 64*3+64*(len_symbol-1)+5);
	
	
				Channel_estimation_d(Output_fft, Output_channel_estimation, Channel, len_symbol, 0);
	
				//Print_complex_Str(Output_channel_estimation, len_symbol*48+5);

	
				Constellation_demap_d(Output_channel_estimation, Output_constellation_demap, mod_type, len_symbol);
		
				//Print_char_StrHex(Output_constellation_demap, 6+len_afterbcc+5);
	
	
				De_interleaver_d(Output_constellation_demap, Output_de_interleaver, mod_type, len_afterbcc);
	
				//Print_char_StrHex(Output_de_interleaver, 6+len_afterbcc+5);
	
	
				Viterbi_d(Output_de_interleaver, Output_viterbi, bcc_rate, len_afterbcc, len_prebcc);
	
				//Print_char_StrHex(Output_viterbi, 3+len_prebcc+5);
	
				Scrambler(Output_viterbi, output_buffer, len_prebcc, len_tail);
				

				return len_prebcc;
			}
		}
	}


}









int main()
{

	int i;
	
	
	char name[30] = "RX_Input.txt";
	Read_input(input_buffer, "RX_Input.txt");

//	for(i=0; i<880; i++)
//		PHY_SW_RX(input_buffer+i, 1, output_buffer);

//	PHY_SW_RX(input_buffer, 440, output_buffer);
//	PHY_SW_RX(input_buffer+440, 440, output_buffer);


	PHY_SW_RX(input_buffer, 1, output_buffer);
	PHY_SW_RX(input_buffer+1, 440, output_buffer);
	PHY_SW_RX(input_buffer+441, 440, output_buffer);
	Print_char_UsrHex(output_buffer, len_prebcc);

	PHY_SW_RX(input_buffer+881, 440, output_buffer);
	PHY_SW_RX(input_buffer+881+440, 440, output_buffer);
	PHY_SW_RX(input_buffer+881+880, 559, output_buffer);

	Print_char_UsrHex(output_buffer, len_prebcc);

	return 0;
}




