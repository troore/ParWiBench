#include "Sync.h"

Sync::Sync(UserPara *pUser)
{
	NIFFT = pUser->NIFFT;
	CPLen = pUser->CPLen;
	NumULSymbSF = pUser->NumULSymbSF;
	NumLayer = pUser->NumLayer;

	/*
	  802.11 preamble structure, a expedient to synchronize a LTE terminal frame
	  
	  |10 short trainings|GI2|T1|T2|GI|SIGNAL|GI|DATA|GI|DATA|---|
	  |+|+|+|+|+|+|+|+|+|+|++|++++|++++|+|++++|+|++++|+|++++|---|
	  |                   |  |    |    | |    |                 |
	  
	  where a '+' contains 16 samples have a duration of 0.8 microseconds with
	  a 20M sampling rate, thus a single samples lasts 0.05 microseconds.
	 */
	PreambleLen = (10 + (2 + 4 + 4) + (1 + 4)) * 16;

	#ifdef TESTBUFSZ
	InBufSz = 10; // for testing
	#else
	InBufSz = NumLayer * NumULSymbSF * (NIFFT + CPLen);
	#endif

	OutBufSz = PreambleLen + InBufSz;

	output_time_synchro = NULL;
}


Sync::Sync(BSPara *pBS)
{
	NumULSymbSF = pBS->NumULSymbSF;
	NIFFT = pBS->NIFFT;
	CPLen = pBS->CPLen;
	NumRxAntenna = pBS->NumRxAntenna;

	syn_state = 0;
	n_signal_output = 0;
	n_data_output = 0;
	
	PreambleLen = (10 + (2 + 4 + 4) + (1 + 4)) * 16;
	
#ifdef TESTBUFSZ
	InBufSz = 10 + PreambleLen;
#else
	InBufSz = PreambleLen + NumRxAntenna * NumULSymbSF * (NIFFT + CPLen);
#endif

	OutBufSz = InBufSz - PreambleLen;

	output_time_synchro = new complex<float>[InBufSz];
}

void Sync::AddPreamble(complex<float> *pInpData, complex<float> *pOutData)
{
		
	int i,j;
	int n_in = 0, n_out = 0;
	
	complex<float> Short_trs[16];
	complex<float> Long_trs[64];

	
	Short_trs[0] = complex<float>(0.046, 0.046);
	Short_trs[1] = complex<float>(-0.132, 0.002);
	Short_trs[2] = complex<float>(-0.013, -0.079);
	Short_trs[3] = complex<float>(0.143, -0.013);
	Short_trs[4] = complex<float>(0.092, 0.000);
	Short_trs[5] = complex<float>(0.143, -0.013);
	Short_trs[6] = complex<float>(-0.013, -0.079);
	Short_trs[7] = complex<float>(-0.132, 0.002);

	Short_trs[8] = complex<float>(0.046, 0.046);
	Short_trs[9] = complex<float>(0.002, -0.132);
	Short_trs[10] = complex<float>(-0.079, -0.013);
	Short_trs[11] = complex<float>(-0.013, 0.143);
	Short_trs[12] = complex<float>(0.000, 0.092);
	Short_trs[13] = complex<float>(-0.013, 0.143);
	Short_trs[14] = complex<float>(-0.079, -0.013);
	Short_trs[15] = complex<float>(0.002, -0.132);



	Long_trs[0] = complex<float>(0.156, 0.000);
	Long_trs[1] = complex<float>(-0.005, -0.120);
	Long_trs[2] = complex<float>(0.040, -0.111);
	Long_trs[3] = complex<float>(0.097, 0.083);
	Long_trs[4] = complex<float>(0.021, 0.028);
	Long_trs[5] = complex<float>(0.060, -0.088);
	Long_trs[6] = complex<float>(-0.115, -0.055);
	Long_trs[7] = complex<float>(-0.038, -0.106);

	Long_trs[8] = complex<float>(0.098, -0.026);
	Long_trs[9] = complex<float>(0.053, 0.004);
	Long_trs[10] = complex<float>(0.001, -0.115);
	Long_trs[11] = complex<float>(-0.137, -0.047);
	Long_trs[12] = complex<float>(0.024, -0.059);
	Long_trs[13] = complex<float>(0.059, -0.015);
	Long_trs[14] = complex<float>(-0.022, 0.161);
	Long_trs[15] = complex<float>(0.119, -0.004);

	Long_trs[16] = complex<float>(0.062, -0.062);
	Long_trs[17] = complex<float>(0.037, 0.098);
	Long_trs[18] = complex<float>(-0.057, 0.039);
	Long_trs[19] = complex<float>(-0.131, 0.065);
	Long_trs[20] = complex<float>(0.082, 0.092);
	Long_trs[21] = complex<float>(0.070, 0.014);
	Long_trs[22] = complex<float>(-0.060, 0.081);
	Long_trs[23] = complex<float>(-0.056, -0.022);

	Long_trs[24] = complex<float>(-0.035, -0.151);
	Long_trs[25] = complex<float>(-0.122, -0.017);
	Long_trs[26] = complex<float>(-0.127, -0.021);
	Long_trs[27] = complex<float>(0.075, -0.074);
	Long_trs[28] = complex<float>(-0.003, 0.054);
	Long_trs[29] = complex<float>(-0.092, 0.115);
	Long_trs[30] = complex<float>(0.092, 0.106);
	Long_trs[31] = complex<float>(0.012, 0.098);

	Long_trs[32] = complex<float>(-0.156, 0.000);
	Long_trs[33] = complex<float>(0.012, -0.098);
	Long_trs[34] = complex<float>(0.092, -0.106);
	Long_trs[35] = complex<float>(-0.092, -0.115);
	Long_trs[36] = complex<float>(-0.003, -0.054);
	Long_trs[37] = complex<float>(0.075, 0.074);
	Long_trs[38] = complex<float>(-0.127, 0.021);
	Long_trs[39] = complex<float>(-0.122, 0.017);

	Long_trs[40] = complex<float>(-0.035, 0.151);
	Long_trs[41] = complex<float>(-0.056, 0.022);
	Long_trs[42] = complex<float>(-0.060, -0.081);
	Long_trs[43] = complex<float>(0.070, -0.014);
	Long_trs[44] = complex<float>(0.082, -0.092);
	Long_trs[45] = complex<float>(-0.131, -0.065);
	Long_trs[46] = complex<float>(-0.057, -0.039);
	Long_trs[47] = complex<float>(0.037, -0.098);

	Long_trs[48] = complex<float>(0.062, 0.062);
	Long_trs[49] = complex<float>(0.119, 0.004);
	Long_trs[50] = complex<float>(-0.022, -0.161);
	Long_trs[51] = complex<float>(0.059, 0.015);
	Long_trs[52] = complex<float>(0.024, 0.059);
	Long_trs[53] = complex<float>(-0.137, 0.047);
	Long_trs[54] = complex<float>(0.001, 0.115);
	Long_trs[55] = complex<float>(0.053, -0.004);

	Long_trs[56] = complex<float>(0.098, 0.026);
	Long_trs[57] = complex<float>(-0.038, 0.106);
	Long_trs[58] = complex<float>(-0.115, 0.055);
	Long_trs[59] = complex<float>(0.060, 0.088);
	Long_trs[60] = complex<float>(0.021, -0.028);
	Long_trs[61] = complex<float>(0.097, -0.083);
	Long_trs[62] = complex<float>(0.040, 0.111);
	Long_trs[63] = complex<float>(-0.005, 0.120);
	
	/*
	 * Add 10 short trainings, each containing 16 complex symbols
	 */
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 16; j++)
		{
			if (i == 0 && j == 0)
				pOutData[n_out++] = complex<float>(0.23, 0.23);
			else
				pOutData[n_out++] = Short_trs[j];
		}
	}
	
	/*
	 * Add long trainings
	 */
	// GI part
	pOutData[n_out++] = complex<float>(-0.055, 0.023);
	
	for (i = 33; i < 64; i++)
	{
		pOutData[n_out++] = Long_trs[i];
	}

	// two trainings part
	for (i = 0; i < 64; i++)
	{
		pOutData[n_out++] = Long_trs[i];
	}
		
	for (i = 0; i < 64; i++)
	{
		pOutData[n_out++] = Long_trs[i];
	}

	// signal part: vacant currently
	for (i = 0; i < (16 + 64); i++)
	{
		pOutData[n_out++] = complex<float>(1.0, -1.0);
	}

	/*
	 * Data
	 */
	for (i = 0; i < InBufSz; i++)
	{
		pOutData[n_out++] = pInpData[n_in++];
	}
}

int Sync::TimeSync(complex<float> input, complex<float> *Output, int num_th, int m_th, int mf_th)
{
	int i, n;
	int n_out;
	
	complex<float> c, d;
	float p, m, mf;
	
	float f_mth;
	float f_mfth;
	
	static int count = 0;
	static int syc = 0;
	static int state = 0;
	static int p_reg = 0;
	static int count_m = 0;
	static int count_mf = 0;
	
	n_out = 0;

	
	static complex<float> Shift[32] = {
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0)
	};

	// The reserved training at receiver
	static complex<float> Short_trs[16] = {
		complex<float>(0.002, -0.132),
		complex<float>(-0.079, -0.013),
		complex<float>(-0.013, 0.143),
		complex<float>(0.000, 0.092),
		complex<float>(-0.013, 0.143),
		complex<float>(-0.079, -0.013),
		complex<float>(0.002, -0.132),
		complex<float>(0.046, 0.046),
									
		complex<float>(-0.132, 0.002),
		complex<float>(-0.013, -0.079),
		complex<float>(0.143, -0.013),
		complex<float>(0.092, 0.000),
		complex<float>(0.143, -0.013),
		complex<float>(-0.013, -0.079),
		complex<float>(-0.132, 0.002),
		complex<float>(0.046, 0.046)						
	};
	
	static complex<float> Reg[96] = {
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
								
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
								
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
								
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0),
		complex<float>(0.0, 0.0)
	};
								
								
	
	f_mth = (float)m_th/1000.0;
	f_mfth = (float)mf_th/1000.0;
	
	
	//Every loop read only one data.
	//Read input and do some shift count!
	for(i=31; i>=1; i--)
	{
		Shift[i] = Shift[i-1];
	}
	Shift[0] = input;
	
	Reg[p_reg] = Shift[0];
	p_reg = (p_reg+1) % 96; //store the datas to the reg.
	
	
	//Start to count the para
	c = complex<float>(0.0, 0.0);
	for (i = 0; i < 16; i++)
	{
		//	c = Complex_add(c, Complex_mul_conj(Shift[i], Shift[i+16]));
		c = c + Shift[i] * conj(Shift[i + 16]);
	}
	
	d = complex<float>(0.0, 0.0);
	for (i = 0; i < 16; i++)
	{
		//	d = Complex_add(d, Complex_mul_conj(Short_trs[i], Shift[i+16]));
		d = d + Short_trs[i] * conj(Shift[i + 16]);
	}
	
	p = 0.0;
	for(i=16; i<32; i++)
	{
		//	p = p + Shift[i].real*Shift[i].real + Shift[i].imag*Shift[i].imag;
		p = p + real(Shift[i]) * real(Shift[i]) + imag(Shift[i]) * imag(Shift[i]);
	}
	
	if(p == 0)
	{
		m = 0.0;
		mf = 0.0;
	}
	else
	{
		//m = (c.real*c.real + c.imag*c.imag) / (p*p);
		//	m = (((c.real > 0) ? c.real : (0.0-c.real)) + ((c.imag > 0) ? c.imag : (0.0-c.imag)))/p;
		m = (((real(c) > 0) ? real(c) : (0.0 - real(c))) + ((imag(c) > 0) ? imag(c) : (0.0 - imag(c)))) / p;
		
		//	mf = (d.real*d.real + d.imag*d.imag) * 5.0 / p;
		mf = (real(d) * real(d) + imag(d) * imag(d)) * 5.0 / p;
	}
	
	
	
	if(state == 0)
	{ // 0: not synchronized
		if(m>f_mth)
			count++;
		else
			count = 0;
	
	
		if(count >= num_th)
		{
			state = 1;
			count = 0;
		}
	}
	else if(state == 1)
	{ // 1: frame synchronized
		if(m < f_mth)
			count++;
		else
			count_m = count_m + 1;

		if(mf > f_mfth)
		{
			if(count_m > 32)
				count_mf = 0;
			else
				count_mf = count_mf + 1;
			count = 0;
			count_m = 0;
		}

		if(count>=num_th)
		{
			state = 0;
			count = 0;
			count_m = 0;
			count_mf = 0;
		}
		
		if(m<f_mth&&mf>f_mfth)
		{ // bit synchronized 
			if(count_mf >= 5)
			{
				state = 0;
				count = 0;
				count_m = 0;
				count_mf = 0;
				for(n=0; n<32; n++)
				{
					Shift[n] = complex<float>(0.0, 0.0);
				}
				for(i=0; i<96; i++)
				{
					Output[n_out++] = Reg[p_reg];
					p_reg = (p_reg+1) % 96;
				}
			
				return 1;
			}
			else
			{
				state = 0;
				count = 0;
				count_m = 0;
				count_mf = 0;
				for(n=0; n<32; n++)
				{
					Shift[n] = complex<float>(0.0, 0.0);
				}
			}
		}
	}
	
	return 0;
}

float Sync::GetFreqOffset(complex<float> *Input, complex<float> *Output)
{
}

int Sync::FreqSync(complex<float> *Input, complex<float> *Output, int len_symbol, float freq_offset)
{
}

int Sync::TryDetectPreamble(complex<float> *input_buffer, int in_buf_len, complex<float> *output_buffer)
{
	if (0 == in_buf_len)
		return 0;

	int n_in = 0;
	int count = 0;
	int k;

	/*
	static int sync_state = 0;
	static int n_signal_output = 0;
	static int n_data_output = 0;
	*/

	// FSM
	while (1)
	{
		if (0 == syn_state)
		{
			while (n_in < in_buf_len)
			{
				if (1 == TimeSync(input_buffer[n_in++], output_time_synchro, 32, 500, 500))
				{
					syn_state = 1;
					// Keep the last 5 short trainings and the GI2 fields for
					// channel estimation (only used in 802.11)
					n_signal_output = 96;

					break;
				}
			}
			if (n_in >= in_buf_len)
			{
				return 0;
			}
		}
		else if (1 == syn_state)
		{
			std::cout << "1111111111" << std::endl;
			/*
			 * '320' ranges from the fifth short trainings to end of SIGNAL field
			 */
			if (n_signal_output + in_buf_len - n_in < 320)
			{
				while (n_in < in_buf_len)
				{
					output_time_synchro[n_signal_output++] = input_buffer[n_in++];
				}

				return 0;
			}
			else
			{
				while (n_signal_output < 320)
				{
					output_time_synchro[n_signal_output++] = input_buffer[n_in++];
				}
			}

			// TODO: Check the correctness of signal field
			if (1)
			{
				syn_state = 2;
				n_data_output = 0;
			}
			else
			{
				syn_state = 0;
			}
		}
		else if (2 == syn_state)
		{
			std::cout << "2222222222" << std::endl;
			// FIXME: Is OutBufSz a right boundry?
			if (n_data_output + in_buf_len - n_in < OutBufSz)
			{
				while (n_in < in_buf_len)
				{
					output_buffer[n_data_output++] = input_buffer[n_in++];
				}

				return 0;
			}
			else
			{
				while (n_data_output < OutBufSz)
				{
					output_buffer[n_data_output++] = input_buffer[n_in++];
				}

				return n_data_output;
			}
		}
	}
}

Sync::~Sync()
{
	if (output_time_synchro)
		delete[] output_time_synchro;
}

