#include "mod.h"
#include "types.h"
#include "defines.h"

#include <iostream>
#include <fstream>
#include <complex>
#include <cstdlib>
#include <cmath>
#include <cstring>

#define N 10000

int main(int argc, char *argv[])
{

	// modulation input
	uint32_T n_bits = 40;
	uint8_T bits[N];

	// modulation output
	uint32_T n_symbols;
	std::complex<float_point> symbols[N];

	// AWGN parameters
	float_point awgn_sigma = 0.193649;
	float_point N0 = 2.0 * (pow(awgn_sigma, 2.0));

	// demodulation input
	std::complex<float_point> recv_symbols[N];
	// demodulation output
	float_point soft_bits[N];
	uint32_T n_recv_bits;

	uint8_T recv_bits[N];

	// number of bit errors
	uint32_T n_err;

	// generate random input
	srand((unsigned)time(NULL));
	for (uint32_T i = 0; i < n_bits; i++)
	{
		bits[i] = (uint8_T)floor((double)rand() / RAND_MAX + 0.5);
		//	bits[i] = 0;
	}
	
	// file dump
	std::ofstream tx_out;

	tx_out.open("tx.dat");
	if (!tx_out)
	{
		std::cerr << "error: unable to open input file: "
				  << tx_out << std::endl;

		return -1;
	}
	for (uint32_T i = 0; i < n_bits; i++)
	{
		tx_out << (int)(bits[i]);
	}
	tx_out.close();

	Modulator *p_mod = new QPSK();

	// modulation
	p_mod->modulate_bits(bits, n_bits, symbols, n_symbols);

	// channel
	memcpy(recv_symbols, symbols, n_symbols * sizeof(std::complex<float_point>));

	// demodulation
	p_mod->demodulate_soft_bits(recv_symbols, n_symbols, soft_bits, n_recv_bits, N0);

	for (uint32_T i = 0; i < n_recv_bits; i++)
	{
		recv_bits[i] = (soft_bits[i] > 0.0) ? 0 : 1;
	}
	/*
	p_mod->demodulate_bits(recv_symbols, n_symbols, recv_bits, n_recv_bits);
	*/

	n_err = 0;
	for (uint32_T i = 0; i < n_recv_bits; i++)
	{
		if (recv_bits[i] != bits[i])
			n_err++;
	}
	std::cout << "Number of error bits is: " << n_err << std::endl;

	// file dump
	// file dump
	std::ofstream rx_out;

	rx_out.open("rx.dat");
	if (!rx_out)
	{
		std::cerr << "error: unable to open input file: "
				  << rx_out << std::endl;

		return -1;
	}
	for (uint32_T i = 0; i < n_recv_bits; i++)
	{
		rx_out << (int)(recv_bits[i]);
	}
	rx_out.close();

	if (p_mod)
		delete p_mod;
	
	return 0;
}
