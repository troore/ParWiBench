
#include <cstring>
#include <limits>
#include <stdio.h>
#include <omp.h>
#include <immintrin.h>

#include "Turbo.h"

#include "micpower.h"
#include "timer/meas.h"

#define LOG_INFINITY 1e30
#define LEN16 16
/**************Global variables*****************/

int InpBlockShift;
int OutBlockShift;
int NumBlock;
int DataLength;
int LastBlockLength;
	
/* Turbo specific parameters */
int g_gens[N_GENS] = {013, 015};

int g_gen_pols[N_GENS];
int g_rev_gen_pols[N_GENS];
int g_state_trans[N_STATES * 2];
int g_rev_state_trans[N_STATES * 2];
int g_output_parity[N_STATES * (N_GENS - 1) * 2];
int g_rev_output_parity[N_STATES * (N_GENS - 1) * 2];

float Lc;
//float (*com_log)(float a, float b);
void log_decoder_(float *recv_syst,
		float *recv_parity,
		float *apriori,
		float *extrinsic,
		int interleaver_size); 
//! Constant definition to speed up trunc_log() and trunc_exp()
//const double log_double_max = log(std::numeric_limits<double>::max());
//! Constant definition to speed up trunc_log(), trunc_exp() and log_add()
//const double log_double_min = log(std::numeric_limits<double>::min());

/*****************End of globals*****************/

// Turbo Internal Interleaver from 3GPP TS 36.212 v10.0.0 table 5.1.3-3
/*
int TURBO_INT_K_TABLE[TURBO_INT_K_TABLE_SIZE] = {  40,  48,  56,  64,  72,  80,  88,  96, 104, 112,
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
												   5056,5120,5184,5248,5312,5376,5440,5504,5568,5632,
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
*/

__forceinline float max_log(float a, float b)
{
	return (a > b) ? a : b;
}

inline void set_generator_polynomials(int *gens, int n_gens, int constraint_length)
{
	int i, j;
	int K = constraint_length;
	int m = K - 1;
	int n_states = (1 << m);
	int s0, s1, s_prim;
	
	int p0[N_GENS - 1];
	int p1[N_GENS - 1];

	for (i = 0; i < n_gens; i++)
	{
		g_gen_pols[i] = gens[i];
		g_rev_gen_pols[i] = reverse_int(K, gens[i]);
	}

	for (s_prim = 0; s_prim < N_STATES; s_prim++)
	{
//		std::cout << s_prim << std::endl;
		s0 = calc_state_transition(s_prim, 0, p0);
//		std::cout << s0 << "\t";
		g_state_trans[s_prim * 2 + 0] = s0;
		g_rev_state_trans[s0 * 2 + 0] = s_prim;
		for (j = 0; j < (n_gens - 1); j++)
		{
			g_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
			g_rev_output_parity[s0 * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
		}

		s1 = calc_state_transition(s_prim, 1, p1);
//		std::cout << s1 << std::endl;
		g_state_trans[s_prim * 2 + 1] = s1;
		g_rev_state_trans[s1 * 2 + 1] = s_prim;
		for (j = 0; j < (n_gens - 1); j++)
		{
			g_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
			g_rev_output_parity[s1 * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
		}
//		std::cout << (int)p0[0] << "\t" << (int)p1[0] << std::endl;
	}
}

//void internal_interleaver(int in[BLOCK_SIZE], int out[BLOCK_SIZE], int m)
template <typename T>
__forceinline void internal_interleaver(T *in, T *out, int m)
{
    int i;
    int f1 = 0;
    int f2 = 0;
    int idx;
//    double ttime,tbegin;
//    tbegin = dtime();
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
//    ttime = dtime();
//    printf("inter 1 time is %f\n",ttime - tbegin);
    for(i = 0; i < m; i++)
    {
		if ((0 == f1) && (0 == f2))
		{
			idx = i;
		}
		else
		{
			/*
			* There might be a integer overflow error for the calculation of
			* f1 * i + f2 * i * i
			*/
			//	idx = ((f1 * i + f2 * i * i) % m);
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[i] = in[idx];
    }
//    tbegin = dtime();
//    printf("inter 2 time is %f\n",tbegin - ttime);
}


__forceinline void internal_interleaver_(float *in, float *out, int m)
{
	int ii,i;
	int f1 = 0;
	int f2 = 0;
	int idx;
//	double ttime,tbegin;
//	tbegin = dtime();
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
	//	ttime = dtime();
	//	printf("m = %d, inter 1 time is %f\n",m, ttime - tbegin);
	for(i = 0; i < m; i++)
	{
		if ((0 == f1) && (0 == f2))
		{
			idx = i;
		}
		else
		{
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
		out[i] = in[idx];
	}

	//	tbegin = dtime();
	//	printf("inter 2 time is %f\n",tbegin - ttime);
}



//void internal_deinterleaver(int in[BLOCK_SIZE], int out[BLOCK_SIZE], int m)
 template <typename T>
__forceinline void internal_deinterleaver(T *in, T *out, int m)
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


__forceinline void internal_deinterleaver(float *in, float *out, int m)
{
	int ii,i;
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
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
		out[idx] = in[i];

	}
}


__forceinline void constituent_encoder(int *input, int input_len, int *tail, int *parity)
{
	int i, j;
	int encoder_state = 0, target_state;

	for (i = 0; i < input_len; i++)
	{
		for (j = 0; j < (N_GENS - 1); j++)
		{
			parity[i * (N_GENS - 1) + j] = g_output_parity[encoder_state * (N_GENS - 1) * 2 + j * 2 + input[i]];
		}
		encoder_state = g_state_trans[encoder_state * 2 + input[i]];
	}
	// add tail bits to make target_state to be 0
	for (i = 0; i < CST_LEN - 1; i++)
	{
		// Attend the shift direction!!!
		target_state = (encoder_state >> 1) & ((1 << (CST_LEN - 1)) - 1);
		if (g_state_trans[encoder_state * 2 + 0] == target_state)
		{
			tail[i] = 0;
		}
		else
		{
			tail[i] = 1;
		}
		for (j = 0; j < (N_GENS - 1); j++)
		{
			parity[(i + input_len) * (N_GENS - 1) + j] = g_output_parity[encoder_state * (N_GENS - 1) * 2 + j * 2 + tail[i]];
		}
		encoder_state = target_state;
	}
}

void turbo_encoding(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq)
{
	int input_data_length;
	int n_blocks;
	int last_block_length;
	int cur_blk_len;
	int inp_blk_offset, out_bit_offset;
	int i, j, k;
	
	int input_bits[N_UNCODED];
	int interleaved_input_bits[N_UNCODED];
	int syst1[N_UNCODED + N_TAIL];
	int syst2[N_UNCODED + N_TAIL];
	int tail1[N_TAIL];
	int tail2[N_TAIL];
	int parity1[(N_UNCODED + N_TAIL) * (N_GENS - 1)];
	int parity2[(N_UNCODED + N_TAIL) * (N_GENS - 1)];

	set_generator_polynomials(g_gens, N_GENS, CST_LEN);

	input_data_length = lte_phy_params->te_in_buf_sz;
	n_blocks = ((input_data_length + BLOCK_SIZE - 1) / BLOCK_SIZE);
	if (input_data_length % BLOCK_SIZE)
	{
		last_block_length = (input_data_length % BLOCK_SIZE);
	}
	else
	{
		last_block_length = BLOCK_SIZE;
	}

	inp_blk_offset = 0;
	out_bit_offset = 0;
	// encode all code blocks
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? BLOCK_SIZE : last_block_length;

		// encode one code block
		for (j = 0; j < cur_blk_len; j++)
		{
			input_bits[j] = piSeq[inp_blk_offset + j];
		}
		
		encode_block(input_bits, interleaved_input_bits, parity1, tail1, parity2, tail2, cur_blk_len);
		
		/*
		// the first encoder
		constituent_encoder(input_bits, cur_blk_len, tail1, parity1);

		// the interleaver
		internal_interleaver(input_bits, interleaved_input_bits, cur_blk_len);

		// the second encoder
		constituent_encoder(interleaved_input_bits, cur_blk_len, tail2, parity2);
		*/

//		memcpy(syst1, input_bits, sizeof(uint8_T) * m_n_uncoded);
//		memcpy(syst2, interleaved_input_bits, sizeof(uint8_T) * m_n_uncoded);
		for (j = 0; j < cur_blk_len; j++)
		{
			syst1[j] = input_bits[j];
			syst2[j] = interleaved_input_bits[j];
		}
//		memcpy(syst1 + m_n_uncoded, tail1, sizeof(uint8_T) * m_n_tail);
//		memcpy(syst2 + m_n_uncoded, tail2, sizeof(uint8_T) * m_n_tail);

		for (j = cur_blk_len; j < cur_blk_len + N_TAIL; j++)
		{
			syst1[j] = tail1[j - cur_blk_len];
			syst2[j] = tail2[j - cur_blk_len];
		}
		
		// the data part
		for (j = 0; j < cur_blk_len; j++)
		{
			// systematic bits
			pcSeq[out_bit_offset++] = syst1[j];
			// first parity bits
			for (k = 0; k < N_GENS - 1; k++)
			{
				pcSeq[out_bit_offset++] = parity1[j * (N_GENS - 1) + k];
			}
			// second parity bits
			for (k = 0; k < (N_GENS - 1); k++)
			{
				pcSeq[out_bit_offset++] = parity2[j * (N_GENS - 1) + k];
			}
		}
		
		// the first tail
		for (j = 0; j < N_TAIL; j++)
		{
			// first systematic tail bits
			pcSeq[out_bit_offset++] = syst1[cur_blk_len + j];
			// first parity tail bits
			for (k = 0; k < (N_GENS - 1); k++)
			{
				pcSeq[out_bit_offset++] = parity1[(cur_blk_len + j) * (N_GENS - 1) + k];
			}
		}

		// the second tail
		for (j = 0; j < N_TAIL; j++)
		{
			// second systematic tail bits
			pcSeq[out_bit_offset++] = syst2[cur_blk_len + j];
			// second parity tail bits
			for (k = 0; k < (N_GENS - 1); k++)
			{
				pcSeq[out_bit_offset++] = parity2[(cur_blk_len + j) * (N_GENS - 1) + k];
			}
		}

		inp_blk_offset += cur_blk_len;
	}
}

inline void encode_block(int *input_bits,
		int *interleaved_input_bits,
		int *parity1, 
		int *tail1,
		int *parity2,
		int *tail2, 
		int blk_len)
{

	// the first encoder
	constituent_encoder(input_bits, blk_len, tail1, parity1);

	// the interleaver
	internal_interleaver(input_bits, interleaved_input_bits, blk_len);

	/*
	   for (k = 0; k < N_UNCODED; k++)
	   std::cout << interleaved_input_bits[k];
	   std::cout << std::endl;
	   */

	// the second encoder
	constituent_encoder(interleaved_input_bits, blk_len, tail2, parity2);
}


__forceinline int calc_state_transition(int instate, int input, int *parity)
{
	int in = 0;
	int temp = (g_gen_pols[0] & instate), parity_temp, parity_bit;
	int i, j;

	for (i = 0; i < CST_LEN; i++)
	{
		in = (temp & 1) ^ in;
		temp = temp >> 1;
	}
	in = in ^ input;

	for (j = 0; j < (N_GENS - 1); j++)
	{
		parity_temp = (instate | (in << (CST_LEN - 1))) & g_gen_pols[j + 1];
		parity_bit = 0;
		for (i = 0; i < CST_LEN; i++)
		{
			parity_bit = (parity_temp & 1) ^ parity_bit;
			parity_temp = parity_temp >> 1;
		}
		parity[j] = parity_bit;
	}
	
	return (in << (CST_LEN - 2) | (instate >> 1)) & ((1 << (CST_LEN - 1)) - 1);
}

inline int reverse_int(int length, int in)
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

/*
 * FIXME: How to determine output buffer size from
 * input buffer size? But it doesn't matter for now...
 */
void turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits, int n_iters)
{
	int out_data_length;
	int n_blocks;
	int last_block_length;
	int out_bit_offset, out_block_offset;
	int cur_blk_len;
	
	float recv_syst1[N_UNCODED + N_TAIL];
	float recv_syst2[N_UNCODED + N_TAIL];
	float recv_parity1[(N_UNCODED + N_TAIL) * (N_GENS - 1)];
	float recv_parity2[(N_UNCODED + N_TAIL) * (N_GENS - 1)];

	int i, j, k;

	set_generator_polynomials(g_gens, N_GENS, CST_LEN);
	
	out_data_length = lte_phy_params->td_out_buf_sz;
	n_blocks = ((out_data_length + BLOCK_SIZE - 1) / BLOCK_SIZE);
	if (out_data_length % BLOCK_SIZE)
	{
		last_block_length = (out_data_length % BLOCK_SIZE);
	}
	else
	{
		last_block_length = BLOCK_SIZE;
	}

	// Clear data part of recv_syst2
	for (i = 0; i < N_UNCODED; i++)
	{
		recv_syst2[i] = 0.0;
	}

	out_bit_offset = 0;
	out_block_offset = 0;
	int ppOutBits[BLOCK_SIZE];
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? BLOCK_SIZE : last_block_length;
		// data part
		for (j = 0; j < cur_blk_len; j++)
		{
			recv_syst1[j] = pInpData[out_bit_offset++];
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity1[j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
			}
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity2[j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
			}
		}
		// first tail
		for (j = 0; j < N_TAIL; j++)
		{
			recv_syst1[cur_blk_len + j] = pInpData[out_bit_offset++];
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity1[(cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
			}
		}
		// second tail
		for (j = 0; j < N_TAIL; j++)
		{
			recv_syst2[cur_blk_len + j] = pInpData[out_bit_offset++];
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity2[(cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
			}
		}
		decode_block(recv_syst1, recv_syst2, recv_parity1, recv_parity2, pOutBits + out_block_offset, cur_blk_len, n_iters);
		out_block_offset += cur_blk_len;
	}
}


__forceinline void decode_block(float *recv_syst1, 
		float *recv_syst2,
		float *recv_parity1,
		float *recv_parity2,
		int *decoded_bits_i, 
		int interleaver_size,
		int n_iters)
{
	int n_tailed = interleaver_size + N_TAIL;
	int i;
	
	float Le12[BLOCK_SIZE + N_TAIL];
	float Le12_int[BLOCK_SIZE + N_TAIL];
	float Le21[BLOCK_SIZE + N_TAIL];
	float Le21_int[BLOCK_SIZE + N_TAIL];
	float L[BLOCK_SIZE + N_TAIL];

	float int_recv_syst1[BLOCK_SIZE + N_TAIL];
	float deint_recv_syst2[BLOCK_SIZE + N_TAIL];
	float recv_syst[BLOCK_SIZE + N_TAIL];
	float int_recv_syst[BLOCK_SIZE + N_TAIL];

	memset(Le21, 0, n_tailed * sizeof(float));

	internal_interleaver_(recv_syst1, int_recv_syst1, interleaver_size);
	internal_deinterleaver(recv_syst2, deint_recv_syst2, interleaver_size);

	// Combine the results from recv_syst1 and recv_syst2 (in case some bits are transmitted several times)
	for (i = 0; i < interleaver_size; i++)
	{
		recv_syst[i] = recv_syst1[i] + deint_recv_syst2[i];
	}
	for (i = 0; i < interleaver_size; i++)
	{
		int_recv_syst[i] = recv_syst2[i] + int_recv_syst1[i];
	}
	for (i = interleaver_size; i < n_tailed; i++)
	{
		recv_syst[i] = recv_syst1[i];
		int_recv_syst[i] = recv_syst2[i];
	}
//	double log_begin,log_time;
//	log_begin = dtime();
	// do the iterative decoding
	for (i = 0; i < /*MAX_ITERATIONS*/ n_iters; i++)
	{
	//	map_decoder(recv_syst1, recv_parity1, Le21, Le12, interleaver_size);
		
		double tbegin,ttime,energy;
		micpower_start();
		tbegin = dtime();

		for (int h = 0; h < 10000; h++) {
		log_decoder_(recv_syst, recv_parity1, Le21, Le12, interleaver_size);
		}

		ttime = dtime() - tbegin;
		energy = micpower_finalize();
		printf("%lfJ\t%fms\t%fW\n", energy, ttime, (energy * 1000.0) / ttime);
//		log_time = dtime();
//		        printf("log time 1 is %f\n", log_time - log_begin);
		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le21[j] << "\t" << Le12[j] << std::endl;
		*/
			
		internal_interleaver_(Le12, Le12_int, interleaver_size);
//		log_begin = dtime();
//		                        printf("inter time 1 is %f\n", 0 - log_time + log_begin);
		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le12[j] << "\t" << Le12_int[j] << std::endl;
			*/
		memset(Le12_int + interleaver_size, 0, N_TAIL * sizeof(float));
	//	map_decoder(recv_syst2, recv_parity2, Le12_int, Le21_int, interleaver_size);
		log_decoder_(int_recv_syst, recv_parity2, Le12_int, Le21_int, interleaver_size);
		/*
		for (int j = 0; j < interleaver_size; j++)
		{
		//	std::cout << int_recv_syst[j] << "\t" << recv_parity2[j] << std::endl;
			std::cout << Le12_int[j] << "\t" << Le21_int[j] << std::endl;
		}
		*/
		internal_deinterleaver(Le21_int, Le21, interleaver_size);
		memset(Le21 + interleaver_size, 0, N_TAIL);
	}
//	log_time = dtime();
//	printf("log time is %f\n", log_time - log_begin);

	for (i = 0; i < interleaver_size; i++)
	{
		L[i] = recv_syst[i] + Le21[i] + Le12[i];
	//	std::cout << recv_syst1[i] << "\t" << Le21[i] << "\t" << Le12[i] << std::endl;
		decoded_bits_i[i] = (L[i] > 0.0) ? 1 : -1;
	//	decoded_bits_i[i] = (L[i] >= 0.0) ? 1 : 0;
	}
}

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
	__m512 reg;
	float elems[LEN16];
} zmmf_t;
typedef __attribute__((aligned(64))) union zmmi {
	__m512i reg;
	int elems[LEN16];
} zmmi_t;
#endif
/*
void log_decoder(float *recv_syst,
                 float *recv_parity,
                 float *apriori,
                 float *extrinsic,
                 int interleaver_size)
{
    
    float nom, den, temp0, temp1, exp_temp0, exp_temp1, rp;
    int i, j, s0, s1, k, kk, l, s, s_prim, s_prim0, s_prim1;
    int block_length = (interleaver_size + N_TAIL);
    
    float alpha[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
    float beta[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
    float gamma[N_STATES * 2 * (BLOCK_SIZE + N_TAIL + 1)];
    float denom[BLOCK_SIZE + N_TAIL + 1];
    
    Lc = 1.0;
    com_log = max_log;
    
    for (k = 0; k <= block_length; k++)
    {
        denom[k] = -LOG_INFINITY;
    }
    
    // Calculate gamma
    for (k = 1; k <= block_length; k++)
    {
        kk = k - 1;
        
        for (s_prim = 0; s_prim < N_STATES; s_prim++)
        {
            exp_temp0 = 0.0;
            exp_temp1 = 0.0;
            
            for (j = 0; j < (N_GENS - 1); j++)
            {
                rp = recv_parity[kk * (N_GENS - 1) + j];
                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
                {
                    exp_temp0 += rp;
                }
                else
                {
                    exp_temp0 -= rp;
                }
                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
                {
                    exp_temp1 += rp;
                }
                else
                {
                    exp_temp1 -= rp;
                }
            }
            
            gamma[(2 * s_prim + 0) * (block_length + 1) + k] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
            //	std::cout << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
            gamma[(2 * s_prim + 1) * (block_length + 1) + k] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
            //	std::cout << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << std::endl;
        }
    }
    
    // Initiate alpha
    for (int i = 1; i < N_STATES; i++)
    {
        alpha[i * (block_length + 1) + 0] = -LOG_INFINITY;
    }
    alpha[0 * (block_length + 1) + 0] = 0.0;
    
    // Calculate alpha, going forward through the trellis
    for (k = 1; k <= block_length; k++)
    {
        for (s = 0; s < N_STATES; s++)
        {
            s_prim0 = g_rev_state_trans[s * 2 + 0];
            s_prim1 = g_rev_state_trans[s * 2 + 1];
            temp0 = alpha[s_prim0 * (block_length + 1) + k - 1] + gamma[(2 * s_prim0 + 0) * (block_length + 1) + k];
            temp1 = alpha[s_prim1 * (block_length + 1) + k - 1] + gamma[(2 * s_prim1 + 1) * (block_length + 1) + k];
            alpha[s * (block_length + 1) + k] = com_log(temp0, temp1);
            denom[k] = com_log(alpha[s * (block_length + 1) + k], denom[k]);
        }
        
        // Normalization of alpha
        for (l = 0; l < N_STATES; l++)
        {
            alpha[l * (block_length + 1) + k] -= denom[k];
        }
    }
    
    // Initiate beta
    for (i = 1; i < N_STATES; i++)
    {
        beta[i * (block_length + 1) + block_length] = -LOG_INFINITY;
    }
    beta[0 * (block_length + 1) + block_length] = 0.0;
    
    // Calculate beta going backward in the trellis
    for (k = block_length; k >= 2; k--)
    {
        for (s_prim = 0; s_prim < N_STATES; s_prim++)
        {
            s0 = g_state_trans[s_prim * 2 + 0];
            s1 = g_state_trans[s_prim * 2 + 1];
            beta[s_prim * (block_length + 1) + k - 1] = com_log(beta[s0 * (block_length + 1) + k] + gamma[(2 * s_prim + 0) * (block_length + 1) + k], beta[s1 * (block_length + 1) + k] + gamma[(2 * s_prim + 1) * (block_length + 1) + k]);
        }
        // Normalization of beta
        for (l = 0; l < N_STATES; l++)
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
        for (s_prim = 0; s_prim < N_STATES; s_prim++)
        {
            s0 = g_state_trans[s_prim * 2 + 0];
            s1 = g_state_trans[s_prim * 2 + 1];
            exp_temp0 = 0.0;
            exp_temp1 = 0.0;
            for (j = 0; j < (N_GENS - 1); j++)
            {
                rp = recv_parity[kk * (N_GENS - 1) + j];
                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
                {
                    exp_temp0 += rp;
                }
                else
                {
                    exp_temp0 -= rp; 
                }
                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
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
        //	std::cout << nom << "\t" << den << std::endl;
        //	std::cout << extrinsic[kk] << std::endl;
    }
}*/


__forceinline void log_decoder_(float *recv_syst,
		float *recv_parity,
		float *apriori,
		float *extrinsic,
		int interleaver_size)
{

	float nom, den, temp0, temp1, exp_temp0, exp_temp1, rp;
	int i, j, s0, s1, k, kk, l, s, s_prim_, s_prim0, s_prim1;
	int block_length = (interleaver_size + N_TAIL);
	
	float alpha[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
	float beta[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
	float gamma[N_STATES * 2 * (BLOCK_SIZE + N_TAIL + 1)];
	float denom[BLOCK_SIZE + N_TAIL + 1];
//	memset(alpha,0,sizeof(alpha));
//	memset(gamma,0,sizeof(gamma));
//	memset(beta,0,sizeof(beta));
//	memset(denom,0,sizeof(denom));
	Lc = 1.0;
//	com_log = max_log;
	for (k = 1; k <= block_length; k++)	
	{
		kk = k - 1;

		for (s_prim_ = 0; s_prim_ < N_STATES; s_prim_++)
		{
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;

			for (j = 0; j < (N_GENS - 1); j++)
			{
				rp = recv_parity[kk * (N_GENS - 1) + j];
				if (0 == g_output_parity[s_prim_ * (N_GENS - 1) * 2 + j * 2 + 0])
				{
					exp_temp0 += rp;
				}
				else
				{
					exp_temp0 -= rp;
				}
				if (0 == g_output_parity[s_prim_ * (N_GENS - 1) * 2 + j * 2 + 1])
				{
					exp_temp1 += rp;
				}
				else
				{
					exp_temp1 -= rp;
				}
			}

			gamma[(2 * s_prim_ + 0) + 2 * k * N_STATES] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
			//      std::cout << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim_ + 1) + 2 * k * N_STATES] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
			//      std::cout << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << std::endl;
		}
	}
	for (int i = 1; i < N_STATES; i++)
	{
		alpha[i + 0 * N_STATES] = -LOG_INFINITY;
	}
	alpha[0 + 0 * N_STATES] = 0.0;
	for (int i = 1; i < N_STATES; i++)
	{
		beta[i + block_length * N_STATES] = -LOG_INFINITY;
	}
	beta[0 + block_length * N_STATES] = 0.0;
	// Calculate alpha, going forward through the trellis
	zmmi_t block_vector,zero_one_vector,two_vector,N_STATES_vector,num_vector;
	for(int tmpx=0;tmpx<16;tmpx++)
	{
		N_STATES_vector.elems[tmpx] = N_STATES;
		block_vector.elems[tmpx] = block_length + 1;
		zero_one_vector.elems[tmpx] = tmpx%2;
		two_vector.elems[tmpx] = 2;
		num_vector.elems[tmpx] = tmpx;
	}
	for (k = 1; k <= block_length; k++)
	{
		zmmi_t s_prim,tmp1,tmp2,tmp3,k_vector,k_1_vector;
		zmmf_t aa,gg,tt_,tt_0,tt_1;
		float tt1,tt2;
		float *ar,*ga;
		s_prim.elems[0:16] = g_rev_state_trans[0:16];
		for(int tmpx = 0;tmpx<16;tmpx++)
		{
			k_vector.elems[tmpx] = 2 * k;
			k_1_vector.elems[tmpx] = (k - 1);
		}
		tmp1.reg = _mm512_fmadd_epi32(k_1_vector.reg,N_STATES_vector.reg,s_prim.reg);
		//get alpha[s_prim + (k-1) * N_STATES]
		tmp3.reg = _mm512_fmadd_epi32(two_vector.reg,s_prim.reg,zero_one_vector.reg);
		//get gamma[2*s_prim + 0/1 + 2 * k * N_STATES]
		tmp2.reg = _mm512_fmadd_epi32(N_STATES_vector.reg,k_vector.reg,tmp3.reg);
		ar = &alpha[0];
		ga = &gamma[0];
		aa.reg = _mm512_i32gather_ps(tmp1.reg,ar,4);
		gg.reg = _mm512_i32gather_ps(tmp2.reg,ga,4);
		tt_.reg = _mm512_add_ps(aa.reg,gg.reg);
		tt_0.elems[0] = tt_.elems[0];
		tt_1.elems[0] = tt_.elems[1];
		tt_0.elems[1] = tt_.elems[2];
		tt_1.elems[1] = tt_.elems[3];
		tt_0.elems[2] = tt_.elems[4];
		tt_1.elems[2] = tt_.elems[5];
		tt_0.elems[3] = tt_.elems[6];
		tt_1.elems[3] = tt_.elems[7]; 
		tt_0.elems[4] = tt_.elems[8];
		tt_1.elems[4] = tt_.elems[9];
		tt_0.elems[5] = tt_.elems[10];
		tt_1.elems[5] = tt_.elems[11];
		tt_0.elems[6] = tt_.elems[12];
		tt_1.elems[6] = tt_.elems[13];
		tt_0.elems[7] = tt_.elems[14];
		tt_1.elems[7] = tt_.elems[15];
		aa.reg = _mm512_gmax_ps(tt_0.reg,tt_1.reg);
		for(s = 0; s < N_STATES; s++)
		{
			alpha[s + k * N_STATES] = aa.elems[s];//com_log(tt_.elems[s*2+0], tt_.elems[s*2+1]);
			denom[k] =max_log(alpha[s + k * N_STATES], denom[k]);
		}
	// Normalization of alpha
		for (l = 0; l < N_STATES; l++)
		{
			alpha[l + k * N_STATES] -= denom[k];
		}
	}
//	ttime = dtime();
//	printf("alpha time is %f\n",ttime - tbegin);
	// Initiate beta
	
	// Calculate beta going backward in the trellis
	for (k = block_length; k >= 2; k--)
	{
		zmmi_t s_prim,tmp1,tmp2,tmp3,k_vector,k_1_vector;
		zmmf_t aa,gg,tt_,tt_0,tt_1;
		float tt1,tt2;
		float *ar,*ga;
		s_prim.elems[0:16] = g_state_trans[0:16];
		for(int tmpx = 0;tmpx<16;tmpx++)
		{
			k_vector.elems[tmpx] = 2 * k;
			k_1_vector.elems[tmpx] = k;
		}
		tmp1.reg = _mm512_fmadd_epi32(k_1_vector.reg,N_STATES_vector.reg,s_prim.reg);
		//get alpha[s_prim + (k-1) * N_STATES]
		//tmp3.reg = _mm512_fmadd_epi32(two_vector.reg,num_vector.reg,zero_one_vector.reg);
		//get gamma[2*s_prim + 0/1 + 2 * k * N_STATES]
		tmp2.reg = _mm512_fmadd_epi32(N_STATES_vector.reg,k_vector.reg,num_vector.reg);
		ar = &beta[0];
		ga = &gamma[0];
		aa.reg = _mm512_i32gather_ps(tmp1.reg,ar,4);
		gg.reg = _mm512_i32gather_ps(tmp2.reg,ga,4);
		tt_.reg = _mm512_add_ps(aa.reg,gg.reg);
		tt_0.elems[0] = tt_.elems[0];
		tt_1.elems[0] = tt_.elems[1];
		tt_0.elems[1] = tt_.elems[2];
		tt_1.elems[1] = tt_.elems[3];
		tt_0.elems[2] = tt_.elems[4];
		tt_1.elems[2] = tt_.elems[5];
		tt_0.elems[3] = tt_.elems[6];
		tt_1.elems[3] = tt_.elems[7];
		tt_0.elems[4] = tt_.elems[8];
		tt_1.elems[4] = tt_.elems[9];
		tt_0.elems[5] = tt_.elems[10];
		tt_1.elems[5] = tt_.elems[11];
		tt_0.elems[6] = tt_.elems[12];
		tt_1.elems[6] = tt_.elems[13];
		tt_0.elems[7] = tt_.elems[14];
		tt_1.elems[7] = tt_.elems[15];
		aa.reg = _mm512_gmax_ps(tt_0.reg,tt_1.reg);
		for (s_prim_ = 0; s_prim_ < N_STATES; s_prim_++)
		{
			//s0 = g_state_trans[s_prim_ * 2 + 0];
			//s1 = g_state_trans[s_prim_ * 2 + 1];
			//if(tmp2.elems[s_prim_*2]!=(2 * s_prim_ + 0) + 2 * k * N_STATES) printf("wrong\n");
			//if(gg.elems[s_prim_*2]!=gamma[(2 * s_prim_ + 0) + 2 * k * N_STATES]) printf("wrong\n");
			//if((tt_0.elems[s_prim_] != beta[s0 + k * N_STATES] + gamma[(2 * s_prim_ + 0) + 2 * k * N_STATES])||(tt_1.elems[s_prim_] != beta[s1 + k * N_STATES] + gamma[(2 * s_prim_ + 1) + 2 * k * N_STATES])) printf("wrong");
			beta[s_prim_ + (k - 1) * N_STATES] = aa.elems[s_prim_];//com_log(beta[s0 + k * N_STATES] + gamma[(2 * s_prim_ + 0) + 2 * k * N_STATES], beta[s1 + k * N_STATES] + gamma[(2 * s_prim_ + 1) + 2 * k * N_STATES]);
		}
		// Normalization of beta
		for (l = 0; l < N_STATES; l++)
		{
			beta[l + (k - 1) * N_STATES] -= denom[k];
		}
	}
	for (k = 1; k <= block_length; k++)	 
	{
		kk = k - 1;
		nom = -LOG_INFINITY;
		den = -LOG_INFINITY;
		for (s_prim_ = 0; s_prim_ < N_STATES; s_prim_++)
		{
			s0 = g_state_trans[s_prim_ * 2 + 0];
			s1 = g_state_trans[s_prim_ * 2 + 1];
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			for (j = 0; j < (N_GENS - 1); j++)
			{
				rp = recv_parity[kk * (N_GENS - 1) + j];
				if (0 == g_output_parity[s_prim_ * (N_GENS - 1) * 2 + j * 2 + 0])
				{
					exp_temp0 += rp;
				}
				else
				{
					exp_temp0 -= rp; 
				}
				if (0 == g_output_parity[s_prim_ * (N_GENS - 1) * 2 + j * 2 + 1])
				{ 
					exp_temp1 += rp;
				}
				else
				{
					exp_temp1 -= rp;
				}
			}
			nom = max_log(nom, alpha[s_prim_ + kk * N_STATES] + 0.5 * exp_temp0 + beta[s0 + k * N_STATES]);
			den = max_log(den, alpha[s_prim_ + kk * N_STATES] + 0.5 * exp_temp1 + beta[s1 + k * N_STATES]);
		}
		extrinsic[kk] = nom - den;
	}
}
