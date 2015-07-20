
#include <cstring>
#include <limits>
#include <stdio.h>

#include <omp.h>

#include "Turbo.h"
#include "util.h"

//#include "micpower.h"
#include "timer/meas.h"

#define LOG_INFINITY 1e30

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
float (*com_log)(float a, float b);

//! Constant definition to speed up trunc_log() and trunc_exp()
//const double log_double_max = log(std::numeric_limits<double>::max());
//! Constant definition to speed up trunc_log(), trunc_exp() and log_add()
//const double log_double_min = log(std::numeric_limits<double>::min());

/*****************End of globals*****************/

float max_log(float a, float b)
{
	return (a > b) ? a : b;
}

void set_generator_polynomials(int *gens, int n_gens, int constraint_length)
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
void internal_interleaver(T *in, T *out, int m)
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
			/*
			* There might be a integer overflow error for the calculation of
			* f1 * i + f2 * i * i
			*/
			//	idx = ((f1 * i + f2 * i * i) % m);
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[i] = in[idx];
    }
}

/*
void internal_interleaver(float *in, float *out, int m)
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
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[i] = in[idx];
    }
}
*/


//void internal_deinterleaver(int in[BLOCK_SIZE], int out[BLOCK_SIZE], int m)
 template <typename T>
 void internal_deinterleaver(T *in, T *out, int m)
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

/*
void internal_deinterleaver(float *in, float *out, int m)
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
			idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m; 
		}
        out[idx] = in[i];
    }
}
*/

void constituent_encoder(int *input, int input_len, int *tail, int *parity)
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

void encode_block(int *input_bits,
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


int calc_state_transition(int instate, int input, int *parity)
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

int reverse_int(int length, int in)
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


void decode_block(float *recv_syst1, 
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

	internal_interleaver(recv_syst1, int_recv_syst1, interleaver_size);
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

	// do the iterative decoding
	for (i = 0; i < /*MAX_ITERATIONS*/ n_iters; i++)
	{
	//	map_decoder(recv_syst1, recv_parity1, Le21, Le12, interleaver_size);

//		double tbegin,ttime,energy;
//		micpower_start();
//		tbegin = dtime();

//		for (int h = 0; h < 10000; h++) {
			log_decoder(recv_syst, recv_parity1, Le21, Le12, interleaver_size);
//		}

//		ttime = dtime() - tbegin;
//		energy = micpower_finalize();
		//	printf("Energy used in %lf\n", energy);
		//	printf("real time is %fms\n", ttime);
//		printf("%lf\t%f\t%f\n", energy, ttime, (energy * 1000.0) / ttime);

		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le21[j] << "\t" << Le12[j] << std::endl;
		*/
			
		internal_interleaver(Le12, Le12_int, interleaver_size);
		/*
		for (int j = 0; j < interleaver_size; j++)
			std::cout << Le12[j] << "\t" << Le12_int[j] << std::endl;
			*/
		memset(Le12_int + interleaver_size, 0, N_TAIL * sizeof(float));
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
		memset(Le21 + interleaver_size, 0, N_TAIL);
	}

	for (i = 0; i < interleaver_size; i++)
	{
		L[i] = recv_syst[i] + Le21[i] + Le12[i];
	//	std::cout << recv_syst1[i] << "\t" << Le21[i] << "\t" << Le12[i] << std::endl;
		decoded_bits_i[i] = (L[i] > 0.0) ? 1 : -1;
	//	decoded_bits_i[i] = (L[i] >= 0.0) ? 1 : 0;
	}
}

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

	omp_set_num_threads(num_threads);
	// Calculate gamma
	/*
#pragma omp parallel shared(block_length, recv_parity, g_output_parity, gamma)
	{
#pragma omp for private(k, kk, s_prim, exp_temp0, exp_temp1, rp, j)
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

			gamma[(2 * s_prim + 0) + k * 2 * N_STATES] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
		//	std::cout << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim + 1) + k * 2 * N_STATES] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
		//	std::cout << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << std::endl;
		}
	}
	}
	*/

//	for (k = 1; k <= block_length; k++)
#pragma omp parallel shared(block_length, recv_parity, g_output_parity, gamma)
	{
#pragma omp for private(i, k, kk, s_prim, exp_temp0, exp_temp1, rp, j)
	for (i = 0; i < block_length * N_STATES; i++)
	{
		k = i / N_STATES + 1;
		kk = k - 1;
		s_prim = i % N_STATES;

		//	for (s_prim = 0; s_prim < N_STATES; s_prim++)
		//	{
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

			gamma[(2 * s_prim + 0) + k * 2 * N_STATES] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
			//	std::cout << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim + 1) + k * 2 * N_STATES] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
			//	std::cout << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << std::endl;
			//	}
	}
	}


	// Initiate alpha
	for (int i = 1; i < N_STATES; i++)
	{
		alpha[i + 0 * N_STATES] = -LOG_INFINITY;
	}
	alpha[0 + 0 * N_STATES] = 0.0;

//	omp_set_num_threads(N_STATES);
	// Calculate alpha, going forward through the trellis
	for (k = 1; k <= block_length; k++)
	{
//#pragma omp parallel shared(g_rev_state_trans, alpha, gamma, k)
		{
//#pragma omp for private(s, s_prim0, s_prim1, temp0, temp1)
		for (s = 0; s < N_STATES; s++)
		{
			s_prim0 = g_rev_state_trans[s * 2 + 0];
			s_prim1 = g_rev_state_trans[s * 2 + 1];
			temp0 = alpha[s_prim0 + (k - 1) * N_STATES] + gamma[(2 * s_prim0 + 0) + k * 2 * N_STATES];
			temp1 = alpha[s_prim1 + (k - 1) * N_STATES] + gamma[(2 * s_prim1 + 1) + k * 2 * N_STATES];
			alpha[s + k * N_STATES] = com_log(temp0, temp1);
			denom[k] = com_log(alpha[s + k * N_STATES], denom[k]);
		}
		}

		// Normalization of alpha
		for (l = 0; l < N_STATES; l++)
		{
			alpha[l + k * N_STATES] -= denom[k];
		}
	}

	// Initiate beta
	for (i = 1; i < N_STATES; i++)
	{
		beta[i + block_length * N_STATES] = -LOG_INFINITY;
	}
	beta[0 + block_length * N_STATES] = 0.0;

	// Calculate beta going backward in the trellis
	for (k = block_length; k >= 2; k--)
	{
		for (s_prim = 0; s_prim < N_STATES; s_prim++)
		{
			s0 = g_state_trans[s_prim * 2 + 0];
			s1 = g_state_trans[s_prim * 2 + 1];
			beta[s_prim + (k - 1) * N_STATES] = com_log(beta[s0 + k * N_STATES] + gamma[(2 * s_prim + 0) + k * 2 * N_STATES], beta[s1 + k * N_STATES] + gamma[(2 * s_prim + 1) + k * 2 * N_STATES]);
		}
		// Normalization of beta
		for (l = 0; l < N_STATES; l++)
		{
			beta[l + (k - 1) * N_STATES] -= denom[k];
		}
	}

//	printf("%d\n", num_threads);

//	omp_set_num_threads(num_threads);
	// Calculate extrinsic output for each bit
#pragma omp parallel shared(block_length, recv_parity, g_output_parity, extrinsic)
	{
#pragma omp for private(k, kk, nom, den, s_prim, s0, s1, exp_temp0, exp_temp1, j, rp)
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
			nom = com_log(nom, alpha[s_prim + kk * N_STATES] + 0.5 * exp_temp0 + beta[s0 + k * N_STATES]);
			den = com_log(den, alpha[s_prim + kk * N_STATES] + 0.5 * exp_temp1 + beta[s1 + k * N_STATES]);
		}
		extrinsic[kk] = nom - den;
	//	std::cout << nom << "\t" << den << std::endl;
	//	std::cout << extrinsic[kk] << std::endl;
	}
	}
}
