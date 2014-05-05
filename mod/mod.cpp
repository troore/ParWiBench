
#include <cstring>

#include "mod.h"

Modulator::Modulator(int32_T m)
{
	
	M = m;
	k = (int32_T)(log(m) / log(2));

	bitmap = new uint8_T[M * k];

	// set bitmap
	uint8_T *bvec = new uint8_T[k];

	for (uint32_T i = 0; i < M; i++)
	{
		dec2bits(i, k, bvec);
		for (uint32_T j = 0; j < k; j++)
		{
			bitmap[i * k + j] = bvec[j];
		}
	}

	delete[] bvec;
}

Modulator::~Modulator()
{
	if (bitmap)
		delete[] bitmap;
}

/*
 * decimal to bit sequence
 * i: decimal
 * n: width of bit sequence
 * bvec: vector to contain the sequence
 */
void Modulator::dec2bits(int32_T i, uint32_T n, uint8_T *bvec) const
{
	for (uint32_T j = 0; j < n; j++)
	{
		bvec[n - j - 1] = (i & 1);
		i = (i >> 1);
	}
}

/*
 * Euclidean distance of two complex values: a+bi and c+di
 */
float_point Modulator::eudist(float_point a, float_point b, float_point c, float_point d) const
{
	return sqrt((a - c) * (a - c) + (b - d) * (b - d));
}


void Modulator::modulate_bits(const uint8_T *bits, uint32_T n_bits, std::complex<float_point> *output, uint32_T &n_signals) const
{
	float_point re, im;

	n_signals = (n_bits / k);
	
	for(uint32_T i = 0; i < n_signals; i++)
	{
		uint32_T idx = 0;
		
		for(uint32_T b = 0; b < k; b++)
		{
			idx += bits[i * k + b] * pow(2.0, (float_point)(k - 1 - b));
		}
		re = m_mod_table[2 * idx + 0];
		im = m_mod_table[2 * idx + 1];
		
		output[i] = std::complex<float>(re, im);
	}
}

float_point Modulator::vec_min(float_point *vec, uint32_T n) const
{
	float_point min_value = vec[0];

	for (uint32_T i = 1; i < n; i++)
	{
		if (vec[i] < min_value)
			min_value = vec[i];
	}

	return min_value;
}

void Modulator::demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const
{
	float_point dist, mindist;
	int32_T closest;
	
	for (uint32_T i = 0; i < n_signals; i++)
	{
		mindist = eudist(m_mod_table[0 * 2 + 0], m_mod_table[0 * 2 + 1], std::real(signals[i]), std::imag(signals[i]));
		
		closest = 0;
		for (uint32_T j = 1; j < M; j++)
		{
			//	dist = std::abs(symbols(j) - signal(i));
			dist = eudist(m_mod_table[j * 2 + 0], m_mod_table[j * 2 + 1], std::real(signals[i]), std::imag(signals[i]));
			if (dist < mindist)
			{
				mindist = dist;
				closest = j;
			}
		}
		//	bits.replace_mid(i*k, bitmap.get_row(closest));
		for (uint32_T j = 0; j < k; j++)
		{
			bits[i * k + j] = bitmap[closest * k + j];
		}
	}

	n_bits = k * n_signals;
}

/*
  void Modulator::demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method) const
  {
  }
*/

void Modulator::demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, float_point *soft_bits, uint32_T &n_soft_bits, float_point N0) const
{

	float_point *pMetric = new float_point[M];
	float_point *pMetricSet = new float_point[2 * (M / 2)];
	float_point *pLLR = new float_point[k * n_signals];
	


	for (uint32_T i = 0; i < n_signals; i++)
	{
		// caculate metric
		for (uint32_T j = 0; j < M; j++)
		{
			pMetric[j] = pow(abs(signals[i] - std::complex<float_point>(m_mod_table[2 * j + 0], m_mod_table[2 * j + 1])), 2.0);
		}
		// generate LLR for each bit
		for (uint32_T j = 0; j < k; j++)
		{
			uint32_T idx0 = 0, idx1 = 0;
			float_point min0, min1;

			for (uint32_T idx = 0; idx < M; idx++)
			{
				if (bitmap[idx * k + j] == 0)
				{
					pMetricSet[0 * (M / 2) + idx0] = pMetric[idx];
					idx0++;
				}
				else
				{
					pMetricSet[1 * (M / 2) + idx1] = pMetric[idx];
					idx1++;
				}
			}
			min0 = vec_min(&pMetricSet[0 * (M / 2)], (M / 2));
			min1 = vec_min(&pMetricSet[1 * (M / 2)], (M / 2));
			if (N0 == (float_point)0)
			{
				pLLR[i * k + j] = (min0 - min1);
			}
			else
			{
				pLLR[i * k + j] = ((min0 - min1) / N0);
			}
		}
	}
	
	n_soft_bits = k * n_signals;
	memcpy(soft_bits, pLLR, n_soft_bits * sizeof(float_point));

	delete[] pMetric;
	delete[] pMetricSet;
	delete[] pLLR;
}

BPSK::BPSK()
	: Modulator(2)
{
	m_BPSK_table = new float_point[2 * 2];

	m_BPSK_table[0 * 2 + 0] = +1.0;
	m_BPSK_table[0 * 2 + 1] = +1.0;
	m_BPSK_table[1 * 2 + 0] = -1.0;
	m_BPSK_table[1 * 2 + 1] = -1.0;

	m_mod_table = m_BPSK_table;
	
}

/*
void BPSK::demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const
{
	for (uint32_T i = 0; i < n_signals; i++)
	{
		bits[i] = (std::real(signals[i]) > 0) ? 0 : 1;
	}
	n_bits = n_signals;
}
*/

/*
void BPSK::demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method) const
{
	float_point factor = (4.0 / N0);
	
	for (uint32_T i = 0; i < n_signals; i++)
	{
		soft_bits[i] = factor * std::real(signals[i]);
	}
	n_soft_bits = n_signals;
}
*/

BPSK::~BPSK()
{
	if (m_BPSK_table)
		delete[] m_BPSK_table;
}

QPSK::QPSK()
	: Modulator(4)
{
	m_QPSK_table = new float_point[4 * 2];

	m_QPSK_table[0 * 2 + 0] = +1.0;
	m_QPSK_table[0 * 2 + 1] = +1.0;
	m_QPSK_table[1 * 2 + 0] = +1.0;
	m_QPSK_table[1 * 2 + 1] = -1.0;
	m_QPSK_table[2 * 2 + 0] = -1.0;
	m_QPSK_table[2 * 2 + 1] = +1.0;
	m_QPSK_table[3 * 2 + 0] = -1.0;
	m_QPSK_table[3 * 2 + 1] = -1.0;

	for (uint32_T r = 0; r < 4; r++)
	{
		for (uint32_T c = 0; c < 2; c++)
		{
			m_QPSK_table[r * 2 + c] /= sqrt(2.0);
		}
	}

	m_mod_table = m_QPSK_table;
}

/*
void QPSK::demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const
{
}
*/

 /*
void QPSK::demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method) const
{
}
 */

QPSK::~QPSK()
{
	if (m_QPSK_table)
		delete[] m_QPSK_table;
}


