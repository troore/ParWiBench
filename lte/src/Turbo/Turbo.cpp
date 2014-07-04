
#include "Turbo.h"
#include <cstring>
#include <limits>

#define LOG_INFINITY 1e30

//! Constant definition to speed up trunc_log() and trunc_exp()
const double log_double_max = log(numeric_limits<double>::max());
//! Constant definition to speed up trunc_log(), trunc_exp() and log_add()
const double log_double_min = log(numeric_limits<double>::min());


// Turbo Internal Interleaver from 3GPP TS 36.212 v10.0.0 table 5.1.3-3
#define TURBO_INT_K_TABLE_SIZE 188

int TURBO_INT_K_TABLE[TURBO_INT_K_TABLE_SIZE] = {  40/*4*/,  48,  56,  64,  72,  80,  88,  96, 104, 112,
														120, 128, 136, 144, 152, 160, 168, 176, 184, 192,
														200, 208, 216, 224, 232, 240, 248, 256, 264, 272,
														280, 288, 296, 304, 312, 320, 328, 336, 344, 352,
														360, 368, 376, 384, 392, 400, 408, 416, 424, 432,
												   440, 448, 456, 464, 472, 480, 488, 496, 504, 512,
												   528, 544, 560, 576, 592, 608, 624, 640, 656, 672,
												   688, 704, 720, 736, 752, 768, 784, 800, 816, 832,
												   848, 864, 880, 896, 912, 928, 944, 960, 976, 992,
												   1008,1024,1056,1088,1120,1152,1184,1216,1248,1280,
												   1312,1344,1376,1408,1440,1472,1504,1536,1568,1600,
												   1632,1664,1696,1728,1760,1792,1824,1856,1888,1920,
												   1952,1984,2016,2048,2112,2176,2240,2304,2368,2432,
												   2496,2560,2624,2688,2752,2816,2880,2944,3008,3072,
												   3136,3200,3264,3328,3392,3456,3520,3584,3648,3712,
												   3776,3840,3904,3968,4032,4096,4160,4224,4288,4352,
												   4416,4480,4544,4608,4672,4736,4800,4864,4928,4992,
												   5056,5120,5184,5248,5312,5376,5440,5504/*5504*/,5568,5632,
												   5696,5760,5824,5888,5952,6016,6080,6144};

int TURBO_INT_F1_TABLE[TURBO_INT_K_TABLE_SIZE] = {  3,  7, 19,  7,  7, 11,  5, 11,  7, 41,103, 15,  9,
													17,  9, 21,101, 21, 57, 23, 13, 27, 11, 27, 85, 29,
													33, 15, 17, 33,103, 19, 19, 37, 19, 21, 21,115,193,
													21,133, 81, 45, 23,243,151,155, 25, 51, 47, 91, 29,
													29,247, 29, 89, 91,157, 55, 31, 17, 35,227, 65, 19,
													37, 41, 39,185, 43, 21,155, 79,139, 23,217, 25, 17,
													127, 25,239, 17,137,215, 29, 15,147, 29, 59, 65, 55,
													31, 17,171, 67, 35, 19, 39, 19,199, 21,211, 21, 43,
													149, 45, 49, 71, 13, 17, 25,183, 55,127, 27, 29, 29,
													57, 45, 31, 59,185,113, 31, 17,171,209,253,367,265,
													181, 39, 27,127,143, 43, 29, 45,157, 47, 13,111,443,
													51, 51,451,257, 57,313,271,179,331,363,375,127, 31,
													33, 43, 33,477, 35,233,357,337, 37, 71, 71, 37, 39,
													127, 39, 39, 31,113, 41,251, 43, 21, 43, 45, 45,161,
													89,323, 47, 23, 47,263};

int TURBO_INT_F2_TABLE[TURBO_INT_K_TABLE_SIZE] = { 10, 12, 42, 16, 18, 20, 22, 24, 26, 84, 90, 32, 34,
												   108, 38,120, 84, 44, 46, 48, 50, 52, 36, 56, 58, 60,
												   62, 32,198, 68,210, 36, 74, 76, 78,120, 82, 84, 86,
												   44, 90, 46, 94, 48, 98, 40,102, 52,106, 72,110,168,
												   114, 58,118,180,122, 62, 84, 64, 66, 68,420, 96, 74,
												   76,234, 80, 82,252, 86, 44,120, 92, 94, 48, 98, 80,
												   102, 52,106, 48,110,112,114, 58,118, 60,122,124, 84,
												   64, 66,204,140, 72, 74, 76, 78,240, 82,252, 86, 88,
												   60, 92,846, 48, 28, 80,102,104,954, 96,110,112,114,
												   116,354,120,610,124,420, 64, 66,136,420,216,444,456,
												   468, 80,164,504,172, 88,300, 92,188, 96, 28,240,204,
												   104,212,192,220,336,228,232,236,120,244,248,168, 64,
												   130,264,134,408,138,280,142,480,146,444,120,152,462,
												   234,158, 80, 96,902,166,336,170, 86,174,176,178,120,
												   182,184,186, 94,190,480};


float max_log(float a, float b)
{
	return (a > b) ? a : b;
}

float add_log(float a, float b)
{
	if (a < b)
	{
		double tmp = a;
		a = b;
		b = tmp;
	}
	float negdelta = b - a;
	if ((negdelta < log_double_min) || isnan(negdelta))
		return a;
	else
		return (a + log1p(exp(negdelta)));
}

Turbo::Turbo(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;
	
	Rate=3;
	DataLength=(*pUser).DataLength;

	m_n_gens = pUser->n_gens;
	for (int i = 0; i < m_n_gens; i++)
	{
		m_gens[i] = pUser->gens[i];
	}
	m_cst_len = pUser->constraint_length;
	m_block_size = pUser->BlkSize;
	m_n_iters = pUser->n_iterations;

	NumBlock = ((DataLength + m_block_size - 1) / m_block_size);
	if (DataLength % m_block_size)
	{
		LastBlockLength = (DataLength % m_block_size);
	}
	else
	{
		LastBlockLength = m_block_size;
	}
	

	/*
	  piSeq=new int[DataLength];
	  pcSeq=new int*[Rate];
	  for(int r=0;r<Rate;r++)
	  {
	  *(pcSeq+r)=new int[((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4))];
	  }
	*/
	///////////////Calc In/Out buffer size/////////////////
//	InBufSz[0]=1;    InBufSz[1] = DataLength;
	InBufSz = DataLength;
//	OutBufSz[0]=Rate;OutBufSz[1]= ((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4));
	OutBufSz = Rate * ((NumBlock - 1) * (m_block_size + 4) + 1 * (LastBlockLength + 4));
	
	set_parameters(pUser->gens, pUser->n_gens, pUser->constraint_length,
				   pUser->BlkSize, pUser->n_iterations);

	/*
	  if(BufFlag)
	  {
	  cout<<"TurboEncoder"<<endl;
	  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
	  cout<<"Output buffer size is"<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
	  }
	  else
	  {}
	*/
	////////////////End clac in/out buffer size///////////////
	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<int>[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////
}


void Turbo::set_parameters(int *gens, int n_gens, int constraint_length, int block_size, int n_iters)
{
	
	m_n_gens = n_gens;
	m_cst_len = constraint_length;
	m_block_size = block_size;
	m_n_iters = n_iters;
	m_n_states = (1 << (m_cst_len - 1));
	m_n_tail = m_cst_len - 1;

	m_n_uncoded = block_size;
	m_n_coded = (1 + (n_gens - 1) * 2) * m_n_uncoded + 2 * (1 + m_n_gens - 1) * m_n_tail;

	m_gen_pols = new int[m_n_gens];
	m_rev_gen_pols = new int[m_n_gens];


	/*
	  for (uint32_T i = 0; i < m_n_gens; i++)
	  {
	  m_gen_pols[i] = gens[i];
	  }*/
		
	m_state_trans = new int[m_n_states * 2];
	m_rev_state_trans = new int[m_n_states * 2];
	m_output_parity = new int[m_n_states * (m_n_gens - 1) * 2];
	m_rev_output_parity = new int[m_n_states * (m_n_gens - 1) * 2];
	
	set_generator_polynomials(gens, n_gens, constraint_length);
	
//	m_trellis = new int8_T[m_n_states * m_n_states * (1 + m_n_gens)];

	Lc = 1.0;
	com_log = max_log;
//	com_log = add_log;
}

template <typename T>
void Turbo::internal_interleaver(T *in, T *out, int m)
{
    int i;
    int f1 = 0;
    int f2 = 0;
    int idx;

    // Determine f1 and f2
    for(i = 0; i < TURBO_INT_K_TABLE_SIZE; i++)
    {
        if(m == TURBO_INT_K_TABLE[i])
        {
            f1 = TURBO_INT_F1_TABLE[i];
            f2 = TURBO_INT_F2_TABLE[i];
            break;
        }
    }

    for(i = 0; i < m; i++)
    {
		if ((0 == f1) && (0 == f2))
		{
			idx = i;
		}
		else
		{
			//	idx = ((f1 * i + f2 * i * i) % m);
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[i] = in[idx];
    }
}

template <typename T>
void Turbo::internal_deinterleaver(T *in, T *out, int m)
{
    int i;
    int f1 = 0;
    int f2 = 0;
    int idx;

    // Determine f1 and f2
    for (i = 0; i < TURBO_INT_K_TABLE_SIZE; i++)
    {
        if (m == TURBO_INT_K_TABLE[i])
        {
            f1 = TURBO_INT_F1_TABLE[i];
            f2 = TURBO_INT_F2_TABLE[i];
            break;
        }
    }

    for (i = 0; i < m; i++)
    {
		if ((0 == f1) && (0 == f2))
		{
			idx = i;

		}
		else
		{
			//	idx = ((f1 * i + f2 * i * i) % m);
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[idx] = in[i];
    }
}

void Turbo::constituent_encoder(int *input, int input_len, int *tail, int *parity)
{
	int encoder_state = 0, target_state;

	for (int i = 0; i < input_len; i++)
	{
		for (int j = 0; j < (m_n_gens - 1); j++)
		{
			parity[i * (m_n_gens - 1) + j] = m_output_parity[encoder_state * (m_n_gens - 1) * 2 + j * 2 + input[i]];
		}
		encoder_state = m_state_trans[encoder_state * 2 + input[i]];
	}
	// add tail bits to make target_state to be 0
	for (int i = 0; i < m_cst_len - 1; i++)
	{
		// Attend the shift direction!!!
		target_state = (encoder_state >> 1) & ((1 << (m_cst_len - 1)) - 1);
		if (m_state_trans[encoder_state * 2 + 0] == target_state)
		{
			tail[i] = 0;
		}
		else
		{
			tail[i] = 1;
		}
		for (int j = 0; j < (m_n_gens - 1); j++)
		{
			parity[(i + input_len) * (m_n_gens - 1) + j] = m_output_parity[encoder_state * (m_n_gens - 1) * 2 + j * 2 + tail[i]];
		}
		encoder_state = target_state;
	}
}


//void Turbo::TurboEncoding(FIFO<int> *pOutBuf)
void Turbo::TurboEncoding(int *piSeq, int *pcSeq)
{
	int n_blocks = NumBlock;
	int *pLengthSet;
	int iSeqLength, InpBlockShift, OutBitShift;

	n_blocks = NumBlock;
	
	pLengthSet = new int[n_blocks];
	for(int i = 0; i < n_blocks - 1; i++)
	{
		pLengthSet[i] = m_n_uncoded;
	}

	pLengthSet[n_blocks - 1] = (DataLength % m_n_uncoded) ? (DataLength % m_n_uncoded) : m_n_uncoded;
	/*
	if (DataLength % m_n_uncoded)
		pLengthSet[n_blocks - 1] = (1 == n_blocks) ? DataLength : (DataLength % m_n_uncoded);
	else
		pLengthSet[n_blocks - 1] = m_n_uncoded;
	*/

	int *input_bits = new int[m_n_uncoded];
	int *interleaved_input_bits = new int[m_n_uncoded];
	int *syst1 = new int[m_n_uncoded + m_n_tail];
	int *syst2 = new int[m_n_uncoded + m_n_tail];
	int *tail1 = new int[m_n_tail];
	int *tail2 = new int[m_n_tail];
	int *parity1 = new int[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];
	int *parity2 = new int[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];
	
	OutBitShift = 0;
	InpBlockShift = 0;

	// encode all code blocks
	for (int i = 0; i < n_blocks; i++)
	{
		iSeqLength = pLengthSet[i];

		// encode one code block
		for (int j = 0; j < iSeqLength; j++)
		{
			input_bits[j] = piSeq[InpBlockShift + j];
		}

		
		// the first encoder
		constituent_encoder(input_bits, iSeqLength, tail1, parity1);

		// the interleaver
		internal_interleaver(input_bits, interleaved_input_bits, iSeqLength);

		/*
		for (int k = 0; k < (m_n_uncoded); k++)
			cout << interleaved_input_bits[k];
		cout << endl;
		*/

		// the second encoder
		constituent_encoder(interleaved_input_bits, iSeqLength, tail2, parity2);

//		memcpy(syst1, input_bits, sizeof(uint8_T) * m_n_uncoded);
//		memcpy(syst2, interleaved_input_bits, sizeof(uint8_T) * m_n_uncoded);
		for (int j = 0; j < iSeqLength; j++)
		{
			syst1[j] = input_bits[j];
			syst2[j] = interleaved_input_bits[j];
		}
//		memcpy(syst1 + m_n_uncoded, tail1, sizeof(uint8_T) * m_n_tail);
//		memcpy(syst2 + m_n_uncoded, tail2, sizeof(uint8_T) * m_n_tail);

		for (int j = iSeqLength; j < iSeqLength + m_n_tail; j++)
		{
			syst1[j] = tail1[j - iSeqLength];
			syst2[j] = tail2[j - iSeqLength];
		}
		
		// the data part
		for (int j = 0; j < iSeqLength; j++)
		{
			// systematic bits
			pcSeq[OutBitShift++] = syst1[j];
			// first parity bits
			for (int k = 0; k < m_n_gens - 1; k++)
			{
				pcSeq[OutBitShift++] = parity1[j * (m_n_gens - 1) + k];
			}
			// second parity bits
			for (int k = 0; k < (m_n_gens - 1); k++)
			{
				pcSeq[OutBitShift++] = parity2[j * (m_n_gens - 1) + k];
			}
		}
		
		// the first tail
		for (int j = 0; j < m_n_tail; j++)
		{
			// first systematic tail bits
			pcSeq[OutBitShift++] = syst1[iSeqLength + j];
			// first parity tail bits
			for (int k = 0; k < (m_n_gens - 1); k++)
			{
				pcSeq[OutBitShift++] = parity1[(iSeqLength + j) * (m_n_gens - 1) + k];
			}
		}

		// the second tail
		for (int j = 0; j < m_n_tail; j++)
		{
			// second systematic tail bits
			pcSeq[OutBitShift++] = syst2[iSeqLength + j];
			// second parity tail bits
			for (int k = 0; k < (m_n_gens - 1); k++)
			{
				pcSeq[OutBitShift++] = parity2[(iSeqLength + j) * (m_n_gens - 1) + k];
			}
		}

		InpBlockShift += iSeqLength;
	}
	
	delete[] input_bits;
	delete[] interleaved_input_bits;
	delete[] syst1;
	delete[] syst2;
 	delete[] tail1;
	delete[] tail2;
	delete[] parity1;
	delete[] parity2;

	delete[] pLengthSet;
}


int Turbo::calc_state_transition(int instate, int input, int *parity)
{
	int in = 0;
//	int temp = (m_rev_gen_pols[0] & (instate << 1)), parity_temp, parity_bit;
	int temp = (m_gen_pols[0] & instate), parity_temp, parity_bit;

	for (int i = 0; i < m_cst_len; i++)
	{
		in = (temp & 1) ^ in;
		temp = temp >> 1;
	}
	in = in ^ input;

//	parity.set_size(n - 1, false);
	for (int j = 0; j < (m_n_gens - 1); j++)
	{
//		parity_temp = ((instate << 1) + in) & m_rev_gen_pols[j + 1];
		parity_temp = (instate | (in << (m_cst_len - 1))) & m_gen_pols[j + 1];
		parity_bit = 0;
		for (int i = 0; i < m_cst_len; i++)
		{
			parity_bit = (parity_temp & 1) ^ parity_bit;
			parity_temp = parity_temp >> 1;
		}
		parity[j] = parity_bit;
	}
	
//	return in + ((instate << 1) & ((1 << (m_cst_len - 1)) - 1));
	return (in << (m_cst_len - 2) | (instate >> 1)) & ((1 << (m_cst_len - 1)) - 1);
}

int Turbo::reverse_int(int length, int in)
{
	int out = 0;
	int i, j;

	for (i = 0; i < (length >> 1); i++)
	{
		out = out | ((in & (1 << i)) << (length - 1 - (i << 1)));
	}
	for (j = 0; j < (length - i); j++)
	{
		out = out | ((in & (1 << (j + i))) >> ((j << 1) - (length & 1) + 1));
	}
	
	return out;
}

void Turbo::set_generator_polynomials(int gens[], int n_gens, int constraint_length)
{
//	int j;
//	gen_pol = gen;
//	n = gen.size();
	int K = constraint_length;
	int m = K - 1;
	int n_states = (1 << m);
//	rate = 1.0 / n;
	int s0, s1, s_prim;
	
	int *p0 = new int[n_gens - 1];
	int *p1 = new int[n_gens - 1];

//	uint32_T *rev_gen_pols = m_rev_gen_pols;
	
	for (int i = 0; i < n_gens; i++)
	{
		m_gen_pols[i] = gens[i];
		m_rev_gen_pols[i] = reverse_int(K, gens[i]);
	}

//	state_trans.set_size(Nstates, 2, false);
//	rev_state_trans.set_size(Nstates, 2, false);
//	output_parity.set_size(Nstates, 2*(n - 1), false);
//	rev_output_parity.set_size(Nstates, 2*(n - 1), false);

	for (s_prim = 0; s_prim < m_n_states; s_prim++)
	{
//		std::cout << s_prim << std::endl;
		s0 = calc_state_transition(s_prim, 0, p0);
//		std::cout << s0 << "\t";
		m_state_trans[s_prim * 2 + 0] = s0;
		m_rev_state_trans[s0 * 2 + 0] = s_prim;
		for (int j = 0; j < (n_gens - 1); j++)
		{
			m_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
			m_rev_output_parity[s0 * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
		}

		s1 = calc_state_transition(s_prim, 1, p1);
//		std::cout << s1 << std::endl;
		m_state_trans[s_prim * 2 + 1] = s1;
		m_rev_state_trans[s1 * 2 + 1] = s_prim;
		for (int j = 0; j < (n_gens - 1); j++)
		{
			m_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
			m_rev_output_parity[s1 * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
		}
//		std::cout << (int)p0[0] << "\t" << (int)p1[0] << std::endl;
	}

	delete[] p0;
	delete[] p1;
}


Turbo::Turbo(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;
	DataLength = (*pBS).DataLengthPerUser;

	Rate=3;

	m_n_gens = pBS->n_gens;
	for (int i = 0; i < m_n_gens; i++)
	{
		m_gens[i] = pBS->gens[i];
	}
	m_cst_len = pBS->constraint_length;
	m_block_size = pBS->BlkSize;
	m_n_iters = pBS->n_iterations;

	NumBlock = ((DataLength + m_block_size - 1) / m_block_size);
	if (DataLength % m_block_size)
	{
		LastBlockLength = (DataLength % m_block_size);
	}
	else
	{
		LastBlockLength = m_block_size;
	}

//	pLLR = new float[(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12)];
//	pData = new int[DataLength];

//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1;InBufSz[1]=(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12);
	InBufSz = Rate * ((NumBlock - 1) * (m_block_size + 4) + 1 * ((LastBlockLength) + 4));
//	OutBufSz[0]=1;OutBufSz[1]=DataLength;
	OutBufSz = DataLength;
	
	set_parameters(pBS->gens, pBS->n_gens, pBS->constraint_length,
				   pBS->BlkSize, pBS->n_iterations);
	/*
	if(BufFlag)
	{
		cout<<"Turbo Decoder"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  float"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
	}
	else
	{}
	*/
//////////////////////End of clac in/out buffer size//////////////////////
////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<float>[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
//////////////////End of initialization of its own input buffer//////////////////
}

void Turbo::TurboDecoding(float *pInpData, int *pOutBits)
{
	int n_blocks;
	int OutBitShift, OutBlockShift;
	int *pLengthSet;
	int iSeqLength;

	n_blocks = NumBlock;
	
	pLengthSet = new int[n_blocks];
	for(int i = 0; i < n_blocks - 1; i++)
	{
		pLengthSet[i] = m_n_uncoded;
	}
	pLengthSet[n_blocks - 1] = (DataLength % m_n_uncoded) ? (DataLength % m_n_uncoded) : m_n_uncoded;
	
	float *recv_syst1 = new float[m_n_uncoded + m_n_tail];
	float *recv_syst2 = new float[m_n_uncoded + m_n_tail];
	float *recv_parity1 = new float[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];
	float *recv_parity2 = new float[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];

	// Clear data part of recv_syst2
	for (int i = 0; i < m_n_uncoded; i++)
	{
		recv_syst2[i] = 0.0;
	}

	OutBitShift = 0;
	OutBlockShift = 0;

	for (int i = 0; i < n_blocks; i++)
	{
		iSeqLength = pLengthSet[i];
		// data part
		for (int j = 0; j < iSeqLength; j++)
		{
			recv_syst1[j] = pInpData[OutBitShift++];
			for (int k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity1[j * (m_n_gens - 1) + k] = pInpData[OutBitShift++];
			}
			for (int k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity2[j * (m_n_gens - 1) + k] = pInpData[OutBitShift++];
			}
		}
		// first tail
		for (int j = 0; j < m_n_tail; j++)
		{
			recv_syst1[iSeqLength + j] = pInpData[OutBitShift++];
			for (int k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity1[(iSeqLength + j) * (m_n_gens - 1) + k] = pInpData[OutBitShift++];
			}
		}
		// second tail
		for (int j = 0; j < m_n_tail; j++)
		{
			recv_syst2[iSeqLength + j] = pInpData[OutBitShift++];
			for (int k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity2[(iSeqLength + j) * (m_n_gens - 1) + k] = pInpData[OutBitShift++];
			}
		}
		decode_block(recv_syst1, recv_syst2, recv_parity1, recv_parity2, pOutBits + OutBlockShift, iSeqLength);
		OutBlockShift += iSeqLength;
	}

	delete[] recv_syst1;
	delete[] recv_syst2;
	delete[] recv_parity1;
	delete[] recv_parity2;

	delete[] pLengthSet;
}


void Turbo::decode_block(float *recv_syst1, float *recv_syst2,
						 float *recv_parity1, float *recv_parity2, int *decoded_bits_i, int interleaver_size)
{
	int n_tailed = interleaver_size + m_n_tail;
	
//	float_point *apriori = new float_point[n_tailed];
//	float_point *extrinsic = new float_point[n_tailed];
	float *Le12 = new float[n_tailed];
	float *Le12_int = new float[n_tailed];
	float *Le21 = new float[n_tailed];
	float *Le21_int = new float[n_tailed];
	float *L = new float[n_tailed];

	float *int_recv_syst1 = new float[n_tailed];
	float *deint_recv_syst2 = new float[n_tailed];
	float *recv_syst = new float[n_tailed];
	float *int_recv_syst = new float[n_tailed];

	memset(Le21, 0, n_tailed * sizeof(float));

	internal_interleaver(recv_syst1, int_recv_syst1, interleaver_size);
	internal_deinterleaver(recv_syst2, deint_recv_syst2, interleaver_size);

	// Combine the results from recv_syst1 and recv_syst2 (in case some bits are transmitted several times)
	for (int i = 0; i < interleaver_size; i++)
	{
		recv_syst[i] = recv_syst1[i] + deint_recv_syst2[i];
	}
	for (int i = 0; i < interleaver_size; i++)
	{
		int_recv_syst[i] = recv_syst2[i] + int_recv_syst1[i];
	}
	for (int i = interleaver_size; i < n_tailed; i++)
	{
		recv_syst[i] = recv_syst1[i];
		int_recv_syst[i] = recv_syst2[i];
	}

	// do the iterative decoding
	for (int i = 0; i < m_n_iters; i++)
	{
	//	map_decoder(recv_syst1, recv_parity1, Le21, Le12, interleaver_size);
		log_decoder(recv_syst, recv_parity1, Le21, Le12, interleaver_size);
		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le21[j] << "\t" << Le12[j] << std::endl;
			*/
		internal_interleaver(Le12, Le12_int, interleaver_size);
		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le12[j] << "\t" << Le12_int[j] << std::endl;
			*/
		memset(Le12_int + interleaver_size, 0, m_n_tail * sizeof(float));
	//	map_decoder(recv_syst2, recv_parity2, Le12_int, Le21_int, interleaver_size);
		log_decoder(int_recv_syst, recv_parity2, Le12_int, Le21_int, interleaver_size);
		/*
		for (int j = 0; j < interleaver_size; j++)
		{
		//	std::cout << int_recv_syst[j] << "\t" << recv_parity2[j] << std::endl;
			std::cout << Le12_int[j] << "\t" << Le21_int[j] << std::endl;
		}
		*/
		internal_deinterleaver(Le21_int, Le21, interleaver_size);
		memset(Le21 + interleaver_size, 0, m_n_tail);
	}

	for (int i = 0; i < interleaver_size; i++)
	{
		L[i] = recv_syst[i] + Le21[i] + Le12[i];
	//	std::cout << recv_syst1[i] << "\t" << Le21[i] << "\t" << Le12[i] << std::endl;
		decoded_bits_i[i] = (L[i] > 0.0) ? 1 : -1;
	//	decoded_bits_i[i] = (L[i] >= 0.0) ? 1 : 0;
	}

//	delete[] apriori;
//	delete[] extrinsic;
	delete[] Le12;
	delete[] Le12_int;
	delete[] Le21;
	delete[] Le21_int;
	delete[] L;

	delete[] int_recv_syst1;
	delete[] deint_recv_syst2;
	delete[] recv_syst;
	delete[] int_recv_syst;
}

void Turbo::map_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size)
{
	float gamma_k_e, nom, den, temp0, temp1, exp_temp0, exp_temp1;
	int j, s0, s1, k, kk, s, s_prim, s_prim0, s_prim1;
	int block_length = (interleaver_size + m_n_tail);
//	ivec p0, p1;

//	mat alpha(Nstates, block_length + 1);
	float *alpha = new float[m_n_states * (block_length + 1)];
//	mat beta(Nstates, block_length + 1);
	float *beta = new float[m_n_states * (block_length + 1)];
//	mat gamma(2*Nstates, block_length + 1);
	float *gamma = new float[m_n_states * 2 * (block_length + 1)];
//	vec denom(block_length + 1);
	float *denom = new float[block_length + 1];

	//Calculate gamma
//	std::cout << "gamma" << std::endl;
//	std::cout << block_length << std::endl;
	for (k = 1; k <= block_length; k++)
	{
//		std::cout << "k=" << k << std::endl;
		kk = k - 1;
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			
			for (j = 0; j < (m_n_gens - 1); j++)
			{
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0]); /* Is this OK? */
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1]);
			}
			// gamma(2*s_prim+0,k) = std::exp( 0.5*(extrinsic_input(kk) + Lc*rec_systematic(kk))) * std::exp( exp_temp0 );
			// gamma(2*s_prim+1,k) = std::exp(-0.5*(extrinsic_input(kk) + Lc*rec_systematic(kk))) * std::exp( exp_temp1 );
			/* == Changed to trunc_exp() to address bug 1088420 which
			   described a numerical instability problem in map_decode()
			   at high SNR. This should be regarded as a temporary fix and
			   it is not necessarily a waterproof one: multiplication of
			   probabilities still can result in values out of
			   range. (Range checking for the multiplication operator was
			   not implemented as it was felt that this would sacrifice
			   too much runtime efficiency.  Some margin was added to the
			   numerical hardlimits below to reflect this. The hardlimit
			   values below were taken as the minimum range that a
			   "double" should support reduced by a few orders of
			   magnitude to make sure multiplication of several values
			   does not exceed the limits.)

			   It is suggested to use the QLLR based log-domain() decoders
			   instead of map_decode() as they are much faster and more
			   numerically stable.

			   EGL 8/06. == */
			std::cout << "s'=" << s_prim << std::endl;
			std::cout << "apriori[k]=" << apriori[kk] << std::endl;
			gamma[(2 * s_prim + 0) * (block_length + 1) + k] = exp(0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp0);
			std::cout << "(s',0)\t" << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim + 1) * (block_length + 1) + k] = exp(-0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp1);
			std::cout << "(s',1)\t" << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << "\n";
		}
	}

	// initiate alpha
//	alpha.set_col(0, zeros(Nstates));
	for (int i = 0; i < m_n_states; i++)
	{
		alpha[i * (block_length + 1) + 0] = 0.0;
	}
	alpha[0 * (block_length + 1) + 0] = 1.0;

	memset(denom, 0, sizeof(float) * (block_length + 1));

	//Calculate alpha and denom going forward through the trellis
//	std::cout << "alpha" << std::endl;
	for (k = 1; k <= block_length; k++)
	{
//		std::cout << "k=" << k << std::endl;
		for (s = 0; s < m_n_states; s++)
		{
//			std::cout << s << std::endl;
			s_prim0 = m_rev_state_trans[s * 2 + 0];
			s_prim1 = m_rev_state_trans[s * 2 + 1];
//			std::cout << s_prim0 << "\t" << s_prim1 << std::endl;
			temp0 = alpha[s_prim0 * (block_length + 1) + k - 1] * gamma[(2 * s_prim0 + 0) * (block_length + 1) + k];
			temp1 = alpha[s_prim1 * (block_length + 1) + k - 1] * gamma[(2 * s_prim1 + 1) * (block_length + 1) + k];
			alpha[s * (block_length + 1) + k] = temp0 + temp1;
//			std::cout << alpha[s * (block_length + 1) + k] << std::endl;
			denom[k] += temp0 + temp1;
		}
//		std::cout << "denom[" << k << "]=" << denom[k] << std::endl;
		
//		alpha.set_col(k, alpha.get_col(k) / denom(k));
		for (s = 0; s < m_n_states; s++)
		{
			alpha[s * (block_length + 1) + k] /= denom[k];
		}
	}

	// initiate beta
//	beta.set_col(block_length, zeros(Nstates));
//	beta(0, block_length) = 1.0;
	for (int i = 0; i < m_n_states; i++)
	{
		beta[i * (block_length + 1) + block_length] = 0.0;
	}
	beta[0 * (block_length + 1) + block_length] = 1.0;

	//Calculate beta going backward in the trellis
//	std::cout << "beta" << std::endl;
	for (k = block_length; k >= 2; k--)
	{
//		std::cout << "k=" << k << std::endl;
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			s0 = m_state_trans[s_prim * 2 + 0];
			s1 = m_state_trans[s_prim * 2 + 1];
			temp0 = beta[s0 * (block_length + 1) + k] * gamma[(2 * s_prim + 0) * (block_length + 1) + k];
			temp1 = beta[s1 * (block_length + 1) + k] * gamma[(2 * s_prim + 1) * (block_length + 1) + k];
			beta[s_prim * (block_length + 1) + k - 1] = temp0 + temp1;
//			std::cout << beta[s_prim * (block_length + 1) + k - 1] << std::endl;
		}
//		beta.set_col(k - 1, beta.get_col(k - 1) / denom(k));
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			beta[s_prim * (block_length + 1) + k - 1] /= denom[k];
		}
	}

	//Calculate extrinsic output for each bit
//	std::cout << "extrinsic" << std::endl;
	for (k = 1; k <= block_length; k++)
	{
//		std::cout << "k=" << k << std::endl;
		kk = k - 1;
		nom = 0.0;
		den = 0.0;
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
//			std::cout << s_prim << std::endl;
			s0 = m_state_trans[s_prim * 2 + 0];
			s1 = m_state_trans[s_prim * 2 + 1];
//			std::cout << s0 << "\t" << s1 << std::endl;
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			for (j = 0; j < (m_n_gens - 1); j++)
			{
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0]);
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1]);
			}
			// gamma_k_e = std::exp( exp_temp0 );
			gamma_k_e = exp(exp_temp0);
//			std::cout << alpha[s_prim * (block_length + 1) + k - 1] << "\t" << gamma_k_e << "\t" << beta[s0 * (block_length + 1) + k] << std::endl;
			nom += alpha[s_prim * (block_length + 1) + k - 1] * gamma_k_e * beta[s0 * (block_length + 1) + k];

			// gamma_k_e = std::exp( exp_temp1 );
			gamma_k_e = exp(exp_temp1);
//			std::cout << alpha[s_prim * (block_length + 1) + k - 1] << "\t" << gamma_k_e << "\t" << beta[s1 * (block_length + 1) + k] << std::endl;
			den += alpha[s_prim * (block_length + 1) + k - 1] * gamma_k_e * beta[s1 * (block_length + 1) + k];
		}
		//      extrinsic_output(kk) = std::log(nom/den);
//		std::cout << "nom=" << nom << "\t" << "den=" << den << std::endl;
		extrinsic[kk] = log(nom / den);
//		std::cout << extrinsic[kk] << std::endl;
	}
}


void Turbo::log_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size)
{

	float nom, den, temp0, temp1, exp_temp0, exp_temp1, rp;
	int j, s0, s1, k, kk, l, s, s_prim, s_prim0, s_prim1;
	int block_length = (interleaver_size + m_n_tail);

	float *alpha = new float[m_n_states * (block_length + 1)];
	float *beta = new float[m_n_states * (block_length + 1)];
	float *gamma = new float[m_n_states * 2 * (block_length + 1)];

	float *denom = new float[block_length + 1];

	for (k = 0; k <= block_length; k++)
	{
		denom[k] = -LOG_INFINITY;
	}

	// Calculate gamma
	for (k = 1; k <= block_length; k++)
	{
		kk = k - 1;

		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;

			for (j = 0; j < (m_n_gens - 1); j++)
			{
				rp = recv_parity[kk * (m_n_gens - 1) + j];
				if (0 == m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0])
				{
					exp_temp0 += rp;
				}
				else
				{ 
					exp_temp0 -= rp;
				}
				if (0 == m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1])
				{
					exp_temp1 += rp;
				}
				else
				{
					exp_temp1 -= rp; 
				}
			}

			gamma[(2 * s_prim + 0) * (block_length + 1) + k] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
			gamma[(2 * s_prim + 1) * (block_length + 1) + k] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
		}
	}

	// Initiate alpha
	for (int i = 1; i < m_n_states; i++)
	{
		alpha[i * (block_length + 1) + 0] = -LOG_INFINITY;
	}
	alpha[0 * (block_length + 1) + 0] = 0.0;

	// Calculate alpha, going forward through the trellis
	for (k = 1; k <= block_length; k++)
	{
		for (s = 0; s < m_n_states; s++)
		{
			s_prim0 = m_rev_state_trans[s * 2 + 0];
			s_prim1 = m_rev_state_trans[s * 2 + 1];
			temp0 = alpha[s_prim0 * (block_length + 1) + k - 1] + gamma[(2 * s_prim0 + 0) * (block_length + 1) + k];
			temp1 = alpha[s_prim1 * (block_length + 1) + k - 1] + gamma[(2 * s_prim1 + 1) * (block_length + 1) + k];
			alpha[s * (block_length + 1) + k] = com_log(temp0, temp1);
			denom[k] = com_log(alpha[s * (block_length + 1) + k], denom[k]);
		}

		// Normalization of alpha
		for (l = 0; l < m_n_states; l++)
		{
			alpha[l * (block_length + 1) + k] -= denom[k];
		}
	}

	// Initiate beta
	for (int i = 1; i < m_n_states; i++)
	{
		beta[i * (block_length + 1) + block_length] = -LOG_INFINITY;
	}
	beta[0 * (block_length + 1) + block_length] = 0.0;

	// Calculate beta going backward in the trellis
	for (k = block_length; k >= 2; k--)
	{
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			s0 = m_state_trans[s_prim * 2 + 0];
			s1 = m_state_trans[s_prim * 2 + 1];
			beta[s_prim * (block_length + 1) + k - 1] = com_log(beta[s0 * (block_length + 1) + k] + gamma[(2 * s_prim + 0) * (block_length + 1) + k], beta[s1 * (block_length + 1) + k] + gamma[(2 * s_prim + 1) * (block_length + 1) + k]);
		}
		// Normalization of beta
		for (l = 0; l < m_n_states; l++)
		{
			beta[l * (block_length + 1) + k - 1] -= denom[k];
		}
	}

	// Calculate extrinsic output for each bit
	for (k = 1; k <= block_length; k++)
	{
		kk = k - 1;
		nom = -LOG_INFINITY;
		den = -LOG_INFINITY;
		for (s_prim = 0; s_prim < m_n_states; s_prim++)
		{
			s0 = m_state_trans[s_prim * 2 + 0];
			s1 = m_state_trans[s_prim * 2 + 1];
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			for (j = 0; j < (m_n_gens - 1); j++)
			{
				rp = recv_parity[kk * (m_n_gens - 1) + j];
				if (0 == m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0])
				{
					exp_temp0 += rp;
				}
				else
				{
					exp_temp0 -= rp; 
				}
				if (0 == m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1])
				{ 
					exp_temp1 += rp;
				}
				else
				{
					exp_temp1 -= rp;
				}
			}
			nom = com_log(nom, alpha[s_prim * (block_length + 1) + kk] + 0.5 * exp_temp0 + beta[s0 * (block_length + 1) + k]);
			den = com_log(den, alpha[s_prim * (block_length + 1) + kk] + 0.5 * exp_temp1 + beta[s1 * (block_length + 1) + k]);
		}
		extrinsic[kk] = nom - den;
	//	std::cout << extrinsic[kk] << endl;
	}
	
	delete[] alpha;
	delete[] beta;
	delete[] gamma;
	delete[] denom;
}

Turbo::~Turbo()
{
	delete[] m_gen_pols;
	delete[] m_rev_gen_pols;
	delete[] m_state_trans;
	delete[] m_rev_state_trans;
	delete[] m_output_parity;
	delete[] m_rev_output_parity;
}
