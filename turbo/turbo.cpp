#include "turbo.h"

#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>

// Turbo Internal Interleaver from 3GPP TS 36.212 v10.0.0 table 5.1.3-3

uint32_T TURBO_INT_K_TABLE[TURBO_INT_K_TABLE_SIZE] = {  40/*4*/,  48,  56,  64,  72,  80,  88,  96, 104, 112,
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

uint32_T TURBO_INT_F1_TABLE[TURBO_INT_K_TABLE_SIZE] = {  3,  7, 19,  7,  7, 11,  5, 11,  7, 41,103, 15,  9,
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

uint32_T TURBO_INT_F2_TABLE[TURBO_INT_K_TABLE_SIZE] = { 10, 12, 42, 16, 18, 20, 22, 24, 26, 84, 90, 32, 34,
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

Turbo::Turbo(uint32_T gens[], uint32_T n_gens, uint32_T constraint_length, uint32_T interleaver_size, uint32_T n_iters)
{
	m_n_gens = n_gens;
	m_cst_len = constraint_length;
	m_interleaver_size = interleaver_size;
	m_n_iters = n_iters;
	m_n_states = (1 << (m_cst_len - 1));
	m_n_tail = m_cst_len - 1;

	m_n_uncoded = interleaver_size;
	m_n_coded = (1 + (n_gens - 1) * 2) * m_n_uncoded + 2 * (1 + m_n_gens - 1) * m_n_tail;


	m_gen_pols = new uint32_T[m_n_gens];
	m_rev_gen_pols = new uint32_T[m_n_gens];


	/*
	for (uint32_T i = 0; i < m_n_gens; i++)
	{
		m_gen_pols[i] = gens[i];
		}*/
		
	m_state_trans = new uint8_T[m_n_states * 2];
	m_rev_state_trans = new uint8_T[m_n_states * 2];
	m_output_parity = new uint8_T[m_n_states * (m_n_gens - 1) * 2];
	m_rev_output_parity = new uint8_T[m_n_states * (m_n_gens - 1) * 2];
	
	set_generator_polynomials(gens, n_gens, constraint_length);
	
	m_trellis = new int8_T[m_n_states * m_n_states * (1 + m_n_gens)];

	Lc = 1.0;

}

template <typename T>
void Turbo::internal_interleaver(const T *in, T *out) const
{
    uint32_T i;
    uint32_T f1 = 0;
    uint32_T f2 = 0;
    uint32_T idx;

    // Determine f1 and f2
    for(i = 0; i < TURBO_INT_K_TABLE_SIZE; i++)
    {
        if(m_interleaver_size == TURBO_INT_K_TABLE[i])
        {
            f1 = TURBO_INT_F1_TABLE[i];
            f2 = TURBO_INT_F2_TABLE[i];
            break;
        }
    }

    for(i = 0; i < m_interleaver_size; i++)
    {
        idx = (f1 * i + f2 * i * i) % m_interleaver_size;
        out[i] = in[idx];
    }
}

template <typename T>
void Turbo::internal_deinterleaver(const T *in, T *out) const
{
    uint32_T i;
    uint32_T f1 = 0;
    uint32_T f2 = 0;
    uint32_T idx;

    // Determine f1 and f2
    for (i = 0; i < TURBO_INT_K_TABLE_SIZE; i++)
    {
        if (m_interleaver_size == TURBO_INT_K_TABLE[i])
        {
            f1 = TURBO_INT_F1_TABLE[i];
            f2 = TURBO_INT_F2_TABLE[i];
            break;
        }
    }

    for (i = 0; i < m_interleaver_size; i++)
    {
        idx = (f1 * i + f2 * i * i) % m_interleaver_size;
        out[idx] = in[i];
    }
}

void Turbo::spec_constituent_encoder(const uint8_T *in_bits, uint8_T *out_bits, uint8_T *fb_bits) const
{
    uint32_T i;
    uint32_T j;
    uint8_T s_reg[4] = {0, 0, 0, 0};
    uint8_T g_array[4] = {1, 1, 0, 1};
    uint8_T fb_array[4] = {0, 0, 1, 1};

    // Convolutionally encode input
    for(i = 0; i < m_n_uncoded; i++)
    {
        // Calculate the feedback bit
        fb_bits[i] = 0;
        for(j = 0; j < m_cst_len; j++)
        {
            fb_bits[i] += s_reg[j] * fb_array[j];
        }
        fb_bits[i] %= 2;

        // Add the next bit to the shift register
        s_reg[0] = (fb_bits[i] + in_bits[i]) % 2;

        // Calculate the output bit
        out_bits[i] = 0;
        for(j = 0; j < m_cst_len; j++)
        {
            out_bits[i] += s_reg[j] * g_array[j];
        }
        out_bits[i] %= 2;
        
        // Sequence the shift register
        for(j = m_cst_len - 1; j > 0; j--)
        {
            s_reg[j] = s_reg[j - 1];
        }
    }

    // Trellis termination
    for(i = m_n_uncoded; i < m_n_uncoded + m_n_tail; i++)
    {
        // Calculate the feedback bit
        fb_bits[i] = 0;
        for(j = 0; j < m_cst_len; j++)
        {
            fb_bits[i] += s_reg[j] * fb_array[j];
        }
        fb_bits[i] %= 2;

        // Add the next bit to the shift register
        s_reg[0] = (fb_bits[i] + fb_bits[i]) % 2;

        // Calculate the output bit
        out_bits[i] = 0;
        for(j = 0; j < m_cst_len; j++)
        {
            out_bits[i] += s_reg[j] * g_array[j];
        }
        out_bits[i] %= 2;
        
        // Sequence the shift register
        for(j = m_cst_len - 1; j > 0; j--)
        {
            s_reg[j] = s_reg[j - 1];
        }
    }
}

void Turbo::spec_encoder(const uint8_T *input, const uint32_T n_input,
						 uint8_T *output, uint32_T &n_output) const
{
	uint32_T count;
	uint32_T n_branch_bits = m_n_uncoded + m_n_tail;
	uint8_T *te_c = new uint8_T[m_n_uncoded];
    uint8_T *te_z = new uint8_T[n_branch_bits];
    uint8_T *te_fb1 = new uint8_T[n_branch_bits];
    uint8_T *te_c_prime = new uint8_T[n_branch_bits];
    uint8_T *te_z_prime = new uint8_T[n_branch_bits];
    uint8_T *te_x_prime = new uint8_T[n_branch_bits];

	n_output = m_n_coded;

	count = 0;

	// encode one code block
	for (uint32_T j = 0; j < m_n_uncoded; j++)
	{
		te_c[j] = input[j];
	}
	// Construct z
	spec_constituent_encoder(input, te_z, te_fb1);

	// Construct c_prime
	internal_interleaver(input, te_c_prime);

	// Construct z_prime
	spec_constituent_encoder(te_c_prime, te_z_prime, te_x_prime);

	// Construct output
	for (uint32_T j = 0; j < m_n_uncoded; j++)
	{
		output[count++] = te_c[j];
		output[count++] = te_z[j];
		output[count++] = te_z_prime[j];
	}

	// Trellis termination
	for (uint32_T j = 0; j < m_n_tail; j++)
	{
		output[count++] = te_fb1[m_n_uncoded + j];
		output[count++] = te_z[m_n_uncoded + j];
	}
	for (uint32_T j = 0; j < m_n_tail; j++)
	{
		output[count++] = te_x_prime[m_n_uncoded + j];
		output[count++] = te_z_prime[m_n_uncoded + j];
	}

	delete[] te_c;
    delete[] te_z;
    delete[] te_fb1;
    delete[] te_c_prime;
    delete[] te_z_prime;
    delete[] te_x_prime;
}

int Turbo::reverse_int(uint32_T length, int in)
{
	int out = 0;
	uint32_T i, j;

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

int Turbo::calc_state_transition(const uint32_T instate, const int input, uint8_T *parity)
{
	int in = 0;
//	int temp = (m_rev_gen_pols[0] & (instate << 1)), parity_temp, parity_bit;
	int temp = (m_gen_pols[0] & instate), parity_temp, parity_bit;

	for (uint32_T i = 0; i < m_cst_len; i++)
	{
		in = (temp & 1) ^ in;
		temp = temp >> 1;
	}
	in = in ^ input;

//	parity.set_size(n - 1, false);
	for (uint32_T j = 0; j < (m_n_gens - 1); j++)
	{
//		parity_temp = ((instate << 1) + in) & m_rev_gen_pols[j + 1];
		parity_temp = (instate | (in << (m_cst_len - 1))) & m_gen_pols[j + 1];
		parity_bit = 0;
		for (uint32_T i = 0; i < m_cst_len; i++)
		{
			parity_bit = (parity_temp & 1) ^ parity_bit;
			parity_temp = parity_temp >> 1;
		}
		parity[j] = parity_bit;
	}
	
//	return in + ((instate << 1) & ((1 << (m_cst_len - 1)) - 1));
	return (in << (m_cst_len - 2) | (instate >> 1)) & ((1 << (m_cst_len - 1)) - 1);
}

void Turbo::set_generator_polynomials(uint32_T gens[], uint32_T n_gens, uint32_T constraint_length)
{
//	int j;
//	gen_pol = gen;
//	n = gen.size();
	uint32_T K = constraint_length;
	uint32_T m = K - 1;
	uint32_T n_states = (1 << m);
//	rate = 1.0 / n;
	int s0, s1, s_prim;
	
	uint8_T *p0 = new uint8_T[n_gens - 1];
	uint8_T *p1 = new uint8_T[n_gens - 1];

//	uint32_T *rev_gen_pols = m_rev_gen_pols;
	
	for (uint32_T i = 0; i < n_gens; i++)
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
		for (uint32_T j = 0; j < (n_gens - 1); j++)
		{
			m_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
			m_rev_output_parity[s0 * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
		}

		s1 = calc_state_transition(s_prim, 1, p1);
//		std::cout << s1 << std::endl;
		m_state_trans[s_prim * 2 + 1] = s1;
		m_rev_state_trans[s1 * 2 + 1] = s_prim;
		for (uint32_T j = 0; j < (n_gens - 1); j++)
		{
			m_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
			m_rev_output_parity[s1 * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
		}
//		std::cout << (int)p0[0] << "\t" << (int)p1[0] << std::endl;
	}

	delete[] p0;
	delete[] p1;
}

void Turbo::constituent_encoder(const uint8_T *input, uint8_T *tail, uint8_T *parity) const
{
	uint32_T encoder_state = 0, target_state;

	for (uint32_T i = 0; i < m_n_uncoded; i++)
	{
		for (uint32_T j = 0; j < (m_n_gens - 1); j++)
		{
			parity[i * (m_n_gens - 1) + j] = m_output_parity[encoder_state * (m_n_gens - 1) * 2 + j * 2 + (int)input[i]];
		}
		encoder_state = m_state_trans[encoder_state * 2 + (int)input[i]];
	}
	// add tail bits to make target_state to be 0
	for (uint32_T i = 0; i < m_cst_len - 1; i++)
	{
		target_state = (encoder_state << 1) & ((1 << (m_cst_len - 1)) - 1);
		if (m_state_trans[encoder_state * 2 + 0] == target_state)
		{
			tail[i] = 0;
		}
		else
		{
			tail[i] = 1;
		}
		for (uint32_T j = 0; j < (m_n_gens - 1); j++)
		{
			parity[(i + m_n_uncoded) * (m_n_gens - 1) + j] = m_output_parity[encoder_state * (m_n_gens - 1) * 2 + j * 2 + (int)tail[i]];
		}
		encoder_state = target_state;
	}
}

void Turbo::encoder(const uint8_T *input, const uint32_T n_input,
					uint8_T *output, uint32_T &n_output) const
{
	uint32_T n_blocks;
	uint32_T count;
	uint8_T *input_bits = new uint8_T[m_n_uncoded];
	uint8_T *interleaved_input_bits = new uint8_T[m_n_uncoded];
	uint8_T *syst1 = new uint8_T[m_n_uncoded + m_n_tail];
	uint8_T *syst2 = new uint8_T[m_n_uncoded + m_n_tail];
	uint8_T *tail1 = new uint8_T[m_n_tail];
	uint8_T *tail2 = new uint8_T[m_n_tail];
	uint8_T *parity1 = new uint8_T[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];
	uint8_T *parity2 = new uint8_T[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];

	n_blocks = n_input / m_n_uncoded;
	n_output = n_blocks * m_n_coded;

	count = 0;

	// encode all code blocks
	for (uint32_T i = 0; i < n_blocks; i++)
	{
		// encode one code block
		for (uint32_T j = 0; j < m_n_uncoded; j++)
		{
			input_bits[j] = input[i * m_n_uncoded + j];
		}

		// the first encoder
		constituent_encoder(input_bits, tail1, parity1);

		// the interleaver
		internal_interleaver(input_bits, interleaved_input_bits);

		// the second encoder
		constituent_encoder(interleaved_input_bits, tail2, parity2);

//		memcpy(syst1, input_bits, sizeof(uint8_T) * m_n_uncoded);
//		memcpy(syst2, interleaved_input_bits, sizeof(uint8_T) * m_n_uncoded);
		for (uint32_T j = 0; j < m_n_uncoded; j++)
		{
			syst1[j] = input_bits[j];
			syst2[j] = interleaved_input_bits[j];
		}
//		memcpy(syst1 + m_n_uncoded, tail1, sizeof(uint8_T) * m_n_tail);
//		memcpy(syst2 + m_n_uncoded, tail2, sizeof(uint8_T) * m_n_tail);

		for (uint32_T j = m_n_uncoded; j < m_n_uncoded + m_n_tail; j++)
		{
			syst1[j] = tail1[j - m_n_uncoded];
			syst2[j] = tail2[j - m_n_uncoded];
		}

		// the data part
		for (uint32_T j = 0; j < m_n_uncoded; j++)
		{
			// systematic bits
			output[count++] = syst1[j];
			// first parity bits
			for (uint32_T k = 0; k < m_n_gens - 1; k++)
			{
				output[count++] = parity1[j * (m_n_gens - 1) + k];
			}
			// second parity bits
			for (uint32_T k = 0; k < (m_n_gens - 1); k++)
			{
				output[count++] = parity2[j * (m_n_gens - 1) + k];
			}
		}
		
		// the first tail
		for (uint32_T j = 0; j < m_n_tail; j++)
		{
			// first systematic tail bits
			output[count++] = syst1[m_n_uncoded + j];
			// first parity tail bits
			for (uint32_T k = 0; k < (m_n_gens - 1); k++)
			{
				output[count++] = parity1[(m_n_uncoded + j) * (m_n_gens - 1) + k];
			}
		}

		// the second tail
		for (uint32_T j = 0; j < m_n_tail; j++)
		{
			// second systematic tail bits
			output[count++] = syst2[m_n_uncoded + j];
			// second parity tail bits
			for (uint32_T k = 0; k < (m_n_gens - 1); k++)
			{
				output[count++] = parity2[(m_n_uncoded + j) * (m_n_gens - 1) + k];
			}
		}
	}

	delete[] input_bits;
	delete[] interleaved_input_bits;
	delete[] syst1;
	delete[] syst2;
	delete[] tail1;
	delete[] tail2;
	delete[] parity1;
	delete[] parity2;
}

void Turbo::gen_trellis()
{
    uint32_T n_states = 8;
    uint8_T s_reg[4] = {0, 0, 0, 0};
    uint8_T g_array[4] = {1, 1, 0, 1};
    uint8_T fb_array[4] = {0, 0, 1, 1};

    // initialization
    for (uint32_T s_prime = 0; s_prime < n_states; s_prime++)
    {
        for (uint32_T s = 0; s < n_states; s++)
        {
            for (uint32_T i = 0; i < 3; i++)
            {
                m_trellis[s_prime * n_states * 3 + s * 3 + i] = 0;
            }
        }
    }
        
    for (uint32_T s_prime = 0; s_prime < n_states; s_prime++)
    {
        for (uint8_T u = 0; u < 2; u++)
        {
            uint32_T s;
            uint8_T c1 = u;
            uint8_T c2 = 0;

            s_reg[0] = u;
            for (uint32_T i = 1; i < 4; i++)
            {
                s_reg[i] = ((s_prime >> (3 - i)) & 1);
            }
            for (uint32_T i = 0; i < 4; i++)
            {
                s_reg[0] += (s_reg[i] * fb_array[i]);
            }
            s_reg[0] %= 2;
            for (uint32_T i = 0; i < 4; i++)
            {
                c2 += (s_reg[i] * g_array[i]);
            }
            c2 %= 2;

            // shift to get s
            s = 0;
            for (uint32_T i = 0; i < 3; i++)
            {
                s |= (s_reg[2 - i] << i);
            }
            
            // trellis[s_prime][s][2] = u
            // c1 = u
            m_trellis[s_prime * n_states * 3 + s * 3 + 0] = 1 - 2 * c1;
            m_trellis[s_prime * n_states * 3 + s * 3 + 1] = 1 - 2 * c2;
            m_trellis[s_prime * n_states * 3 + s * 3 + 2] = 1 - 2 * u;
        }
    }

    /*
    // test trellis
    std::cout << "u\t" << "(s',s)\t" << "c1\t" << "c2\t" << std::endl;
    for (uint32_T s_prime = 0; s_prime < n_states; s_prime++)
    {
	for (uint32_T s = 0; s < n_states; s++)
	{
//       if (trellis[s_prime * n_states * 3 + s * 3 + 2] != 0)
//            {
std::cout << (int)trellis[s_prime * n_states * 3 + s * 3 + 2] << "\t" << "(" << s_prime << "," << s << ")\t" <<
(int)trellis[s_prime * n_states * 3 + s * 3 + 0] << "\t" << (int)trellis[s_prime * n_states * 3 + s * 3 + 1] << "\t" << std::endl;
//            }
}
}
    */
		}

void Turbo::spec_decoder(const float_point *y, const uint32_T n_y, int8_T *d, uint32_T &n_d)
{
    uint32_T n_branch_bits = m_n_uncoded + m_n_tail;
	uint32_T count;

    float_point *y_1s = new float_point[n_branch_bits + 1];
    float_point *y_2s = new float_point[n_branch_bits + 1];
    float_point *y_1p = new float_point[n_branch_bits + 1];
    float_point *y_2p = new float_point[n_branch_bits + 1];

    float_point *apriori = new float_point[n_branch_bits + 1];
    float_point *extrinsic = new float_point[n_branch_bits + 1];
	float_point *extrinsic_int = new float_point[n_branch_bits + 1];
    float_point *LLR = new float_point[n_branch_bits + 1];
    float_point *LLR_int = new float_point[n_branch_bits + 1];

	count = 0;
    for (uint32_T i = 0; i < m_n_uncoded; i++)
    {
        y_1s[i + 1] = y[count++];
        y_1p[i + 1] = y[count++];
		y_2s[i + 1] = 0.0;
        y_2p[i + 1] = y[count++];
    }

	// tail bits
	for (uint32_T i = m_n_uncoded; i < m_n_uncoded + m_n_tail; i++)
	{
		y_1s[i + 1] = y[count++];
		y_1p[i + 1] = y[count++];
	}
	for (uint32_T i = m_n_uncoded; i < m_n_uncoded + m_n_tail; i++)
	{
		y_2s[i + 1] = y[count++];
		y_2p[i + 1] = y[count++];
	}
		
    gen_trellis();

    for (uint32_T i = 1; i <= n_branch_bits; i++)
    {
        apriori[i] = 0.0;
    }
	// interleaver for systematic input bits stream
	internal_interleaver(y_1s + 1, y_2s + 1);

    for (uint32_T i = 0; i < m_n_iters; i++)
    {
        // first siso decoder
        spec_map_decoder(y_1s, y_1p, apriori, extrinsic, LLR, n_branch_bits);

	
		memcpy(extrinsic_int + 1, extrinsic + 1, n_branch_bits * sizeof(float_point));
		/*
		  for (uint32_T j = 0; j < n_d; j++)
		  std::cout << (y_1s + 1)[j] << " ";
		  std::cout << std::endl;
	
		  for (uint32_T j = 0; j < n_d; j++)
		  std::cout << (y_2s + 1)[j] << " ";
		  std::cout << std::endl;
		*/
		
        // interleaver for the first extrinsic bits stream
        internal_interleaver(extrinsic + 1, apriori + 1);
        // second siso decoder
        spec_map_decoder(y_2s, y_2p, apriori, extrinsic, LLR, n_branch_bits);
        // deinterleaver for the second extrinsic bits stream
        internal_deinterleaver(extrinsic + 1, apriori + 1);
        // deinterleaver for the log likelyhood ratio for soft decision
        internal_deinterleaver(LLR + 1, LLR_int + 1);
		
/*		for (uint32_T j = 1; j <= m_n_uncoded; j++)
		{
			d[j - 1] = (y_2s[j] + apriori[j] + extrinsic_int[j] >= 0) ? +1 : -1;
			} */
	
    }
    
    // hard decision for LLR_prime without termination bits
/*	for (uint32_T i = 1; i <= m_n_uncoded; i++)
    {
        d[i - 1] = (LLR_int[i] >= 0) ? +1 : -1;
		} */
	
	
	  for (uint32_T i = 1; i <= m_n_uncoded; i++)
	  {
	  d[i - 1] = (y_2s[i] + apriori[i] + extrinsic_int[i] >= 0) ? +1 : -1;
	  }

	  n_d = m_n_uncoded;
	
    delete[] y_1s;
    delete[] y_2s;
    delete[] y_1p;
    delete[] y_2p;

    delete[] apriori;
    delete[] extrinsic;
	delete[] extrinsic_int;
    delete[] LLR;
    delete[] LLR_int;
}

void Turbo::spec_map_decoder(const float_point *y_s, const float_point *y_p, const float_point *L_apriori,
                                float_point *L_extrinsic, float_point *LLR, const uint32_T n) const
{
    uint32_T n_states = 8;
    // 8 states for turbo encoder constraint length is 4
    float_point *gamma = new float_point[(n + 1) * n_states * n_states];
    float_point *gamma_e = new float_point[(n + 1) * n_states * n_states];
    float_point *alpha = new float_point[(n + 1) * n_states];
    float_point *beta = new float_point[(n + 1) * n_states];

    // initialization for alpha and beta, sentinel setting
    /*
     * s belongs to [0,7]
     * alpha[0][s] = 1, if s == 0
     * alpha[0][s] = 0, otherwise
     *
     * beta[n][s] = 1, if s == 0
     * beta[n][s] = 0, otherwise
     */
    
    for (uint8_T s = 0; s < n_states; s++)
    {
        if (s == 0)
        {
            alpha[0 * n_states + s] = 1;
            beta[n * n_states + s] = 1;
        }
        else
        {
            alpha[0 * n_states + s] = 0;
            beta[n * n_states + s] = 0;
        }
    }

    // zero initialization for gamma and gamma_e,
    // to reduce branch statements when computing alpha, beta, etc.
    for (uint32_T k = 1; k <= n; k++)
    {
        for (uint8_T s_prime = 0; s_prime < n_states; s_prime++)
        {
            for (uint8_T s = 0; s < n_states; s++)
            {
                gamma_e[k * n_states * n_states + s_prime * n_states + s] = 0.0;
                gamma[k * n_states * n_states + s_prime * n_states + s] = 0.0;
            }
        }
    }

    // gamma_e, gamma and alpha
    for (uint32_T k = 1; k <= n; k++)
    {
//        std::cout << "k=" << k << std::endl;
        // gamma_e and gamma
        for (uint8_T s_prime = 0; s_prime < n_states; s_prime++)
        {
            for (uint8_T s = 0; s < n_states; s++)
            {
                // entry trellis[i][j] contains 3 values: c1, c2, u
                if (m_trellis[s_prime * n_states * 3 + s * 3 + 2] != 0)
                {
                    // trellis[i][j] is valid when trellis[i][j][2] != 0
                    // gamma_e[k][i][j]
                    gamma_e[k * n_states * n_states + s_prime * n_states + s] =
                        exp(0.5 * y_p[k] * m_trellis[s_prime * n_states * 3 + s * 3 + 1]);
                    // gamma[k][i][j]
                    gamma[k * n_states * n_states + s_prime * n_states + s] =
                        exp(0.5 * L_apriori[k] * m_trellis[s_prime * n_states * 3 + s * 3 + 0] + 0.5 * y_s[k] * m_trellis[s_prime * n_states * 3 + s * 3 + 0]) *
                        gamma_e[k * n_states * n_states + s_prime * n_states + s];

					/*
					std::cout << L_apriori[k] << "\t" << (int)m_trellis[s_prime * n_states * 3 + s * 3 + 0] << "\t" << y_s[k] << std::endl;

                    std::cout << "(" << (int)s_prime << "," << (int)s << ")\t";
                    std::cout << gamma_e[k * n_states * n_states + s_prime * n_states + s] << "\t" << gamma[k * n_states * n_states + s_prime * n_states + s] << std::endl;
					*/
                }
            }
        }
        
        // denominator for alpha[k]
        float_point denominator = 0.0;
                
        for (uint8_T s = 0; s < n_states; s++)
        {
            for (uint8_T s_prime = 0; s_prime < n_states; s_prime++)
            {
                denominator = denominator + alpha[(k - 1) * n_states + s_prime] *
                    gamma[k * n_states * n_states + s_prime * n_states + s];
            }
        }

        // alpha
        for (uint8_T s = 0; s < n_states; s++)
        {
            // nominator
            float_point nominator = 0.0;
                
            for (uint8_T i = 0; i < n_states; i++)
            {
                nominator = nominator + alpha[(k - 1) * n_states + i] *
                    gamma[k * n_states * n_states + i * n_states + s];
            }
                      
            // normalization
            alpha[k * n_states + s] = nominator / denominator;
        }
    }

    // beta
	std::cout << "beta" << std::endl;
    for (uint32_T k = n; k > 1; k--)
    {
		std::cout << "k=" << k << std::endl;
        // denominator for beta[k]
        float_point denominator = 0.0;
            
        for (uint32_T s = 0; s < n_states; s++)
        {
            for (uint32_T s_prime = 0; s_prime < n_states; s_prime++)
            {
                denominator = denominator + alpha[(k - 2) * n_states + s_prime] *
                    gamma[(k - 1) * n_states * n_states + s_prime * n_states + s];
            }
        }

        for (uint32_T s_prime = 0; s_prime < n_states; s_prime++)
        {
            // nominator
            float_point nominator = 0.0;

            for (uint32_T i = 0; i < n_states; i++)
            {
                nominator = nominator + beta[k * n_states + i] *
                    gamma[k * n_states * n_states + s_prime * n_states + i];
            }

            // nomalization
            beta[(k - 1) * n_states + s_prime] = nominator / denominator;
			std::cout << beta[(k - 1) * n_states + s_prime] << std::endl;
        }
    }

//    std::cout << "L_extrinsic\n";
    // L_extrinsic and LLR
    for (uint32_T k = 1; k <= n; k++)
    {
        float_point nominator = 0.0;
        float_point denominator = 0.0;

        for (uint8_T s_prime = 0; s_prime < n_states; s_prime++)
        {
            for (uint8_T s = 0; s < n_states; s++)
            {
                if (m_trellis[s_prime * n_states * 3 + s * 3 + 2] > 0)
                    nominator = nominator + alpha[(k - 1) * n_states + s_prime] *
                        beta[k * n_states + s] *
                        gamma_e[k * n_states * n_states + s_prime * n_states + s];
                else
                    denominator = denominator + alpha[(k - 1) * n_states + s_prime] *
                        beta[k * n_states + s] *
                        gamma_e[k * n_states * n_states + s_prime * n_states + s];
            }
        }

        L_extrinsic[k] = log(nominator / denominator);

//		std::cout << k << "\t" << L_extrinsic[k] << std::endl;
        // LLR = L_apriori + L_channel + L_extrinsic
        LLR[k] = L_apriori[k] + y_s[k] + L_extrinsic[k];
    }

    delete[] gamma;
    delete[] gamma_e;
    delete[] alpha;
    delete[] beta;
}

void Turbo::map_decoder(float_point *recv_syst, float_point *recv_parity, float_point *apriori, float_point *extrinsic)
{
	float_point gamma_k_e, nom, den, temp0, temp1, exp_temp0, exp_temp1;
	int j, s0, s1, k, kk, s, s_prim, s_prim0, s_prim1;
	int block_length = (m_n_uncoded + m_n_tail);
//	ivec p0, p1;

//	mat alpha(Nstates, block_length + 1);
	float_point *alpha = new float_point[m_n_states * (block_length + 1)];
//	mat beta(Nstates, block_length + 1);
	float_point *beta = new float_point[m_n_states * (block_length + 1)];
//	mat gamma(2*Nstates, block_length + 1);
	float_point *gamma = new float_point[m_n_states * 2 * (block_length + 1)];
//	vec denom(block_length + 1);
	float_point *denom = new float_point[block_length + 1];

	//Calculate gamma
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
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float_point)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0]); /* Is this OK? */
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (float_point)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1]);
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
//			std::cout << "s'=" << s_prim << std::endl;
//			std::cout << "apriori[k]=" << apriori[kk] << std::endl;
			gamma[(2 * s_prim + 0) * (block_length + 1) + k] = exp(0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp0);
//			std::cout << "(s',0)\t" << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim + 1) * (block_length + 1) + k] = exp(-0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp1);
//			std::cout << "(s',1)\t" << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << "\n";
		}
	}

	// initiate alpha
//	alpha.set_col(0, zeros(Nstates));
	for (uint32_T i = 0; i < m_n_states; i++)
	{
		alpha[i * (block_length + 1) + 0] = 0.0;
	}
	alpha[0 * (block_length + 1) + 0] = 1.0;

	memset(denom, 0, sizeof(float_point) * (block_length + 1));

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
	for (uint32_T i = 0; i < m_n_states; i++)
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
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (double)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 0]);
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (m_n_gens - 1) + j] * (double)(1 - 2 * m_output_parity[s_prim * (m_n_gens - 1) * 2 + j * 2 + 1]);
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

void Turbo::decode_block(float_point *recv_syst1, float_point *recv_syst2,
						 float_point *recv_parity1, float_point *recv_parity2, int8_T *decoded_bits_i)
{
	uint32_T n_tailed = m_n_uncoded + m_n_tail;
	
//	float_point *apriori = new float_point[n_tailed];
//	float_point *extrinsic = new float_point[n_tailed];
	float_point *Le12 = new float_point[n_tailed];
	float_point *Le12_int = new float_point[n_tailed];
	float_point *Le21 = new float_point[n_tailed];
	float_point *Le21_int = new float_point[n_tailed];
	float_point *L = new float_point[n_tailed];

	memset(Le21, 0, n_tailed * sizeof(float_point));

	internal_interleaver(recv_syst1, recv_syst2);

	// do the iterative decoding
	for (uint32_T i = 0; i < m_n_iters; i++)
	{
		map_decoder(recv_syst1, recv_parity1, Le21, Le12);
		internal_interleaver(Le12, Le12_int);
		memset(Le12_int + m_interleaver_size, 0, m_n_tail);
		map_decoder(recv_syst2, recv_parity2, Le12_int, Le21_int);
		internal_deinterleaver(Le21_int, Le21);
		memset(Le21 + m_interleaver_size, 0, m_n_tail);
	}
	for (uint32_T i = 0; i < m_n_uncoded; i++)
	{
		L[i] = recv_syst1[i] + Le21[i] + Le12[i];
//		std::cout << L[i] << std::endl;
		decoded_bits_i[i] = (L[i] > 0.0) ? 1 : -1;
	}

//	delete[] apriori;
//	delete[] extrinsic;
	delete[] Le12;
	delete[] Le12_int;
	delete[] Le21;
	delete[] Le21_int;
	delete[] L;
}

void Turbo::decoder(const float_point *recv_signals, const uint32_T n_recv_signals, int8_T *decoded_bits, uint32_T &n_decoded_bits)
{
	uint32_T n_blocks;
	uint32_T count;
	
	float_point *recv_syst1 = new float_point[m_n_uncoded + m_n_tail];
	float_point *recv_syst2 = new float_point[m_n_uncoded + m_n_tail];
	float_point *recv_parity1 = new float_point[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];
	float_point *recv_parity2 = new float_point[(m_n_uncoded + m_n_tail) * (m_n_gens - 1)];

	n_blocks = n_recv_signals / m_n_coded;
	n_decoded_bits = n_blocks * m_n_uncoded;

	count = 0;

	for (uint32_T i = 0; i < n_blocks; i++)
	{
		// data part
		for (uint32_T j = 0; j < m_n_uncoded; j++)
		{
			recv_syst1[j] = recv_signals[count++];
			for (uint32_T k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity1[j * (m_n_gens - 1) + k] = recv_signals[count++];
			}
			for (uint32_T k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity2[j * (m_n_gens - 1) + k] = recv_signals[count++];
			}
		}
		// first tail
		for (uint32_T j = 0; j < m_n_tail; j++)
		{
			recv_syst1[m_n_uncoded + j] = recv_signals[count++];
			for (uint32_T k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity1[(m_n_uncoded + j) * (m_n_gens - 1) + k] = recv_signals[count++];
			}
		}
		for (uint32_T j = 0; j < m_n_tail; j++)
		{
			recv_syst2[m_n_uncoded + j] = recv_signals[count++];
			for (uint32_T k = 0; k < m_n_gens - 1; k++)
			{
				recv_parity2[(m_n_uncoded + j) * (m_n_gens - 1) + k] = recv_signals[count++];
			}
		}
		decode_block(recv_syst1, recv_syst2, recv_parity1, recv_parity2, decoded_bits + i * m_n_uncoded);
	}

	delete[] recv_syst1;
	delete[] recv_syst2;
	delete[] recv_parity1;
	delete[] recv_parity2;
}

Turbo::~Turbo()
{

	delete[] m_gen_pols;
	delete[] m_rev_gen_pols;
	
	delete[] m_state_trans;
	delete[] m_rev_state_trans;
	delete[] m_output_parity;
	delete[] m_rev_output_parity;

	delete[] m_trellis;
}


