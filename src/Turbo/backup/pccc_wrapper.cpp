
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "turbo.h"
#include "types.h"
#include "defines.h"
#include "pccc_wrapper.h"

#include <iostream>
#include <fstream>

//extern "C" {
// turbo encode
void wrapper_turbo_encoder(const uint8_T *c_bits, const uint32_T n_c_bits,
						   uint8_T *d_bits, uint32_T &n_d_bits)
{
	uint32_T n_gens = 2;
	uint32_T gens[2] = {013, 015};
	uint32_T constraint_length = 4;
    Turbo tb(gens, n_gens, constraint_length, n_c_bits);

//	tb.spec_encoder(c_bits, n_c_bits, d_bits, n_d_bits);
	tb.encoder(c_bits, n_c_bits, d_bits, n_d_bits);
}

// tx send
void wrapper_send(uint8_T *d_bits, uint32_T n_d_bits)
{
	// just write to file for now
	std::ofstream f_tx;

	f_tx.open("tx_channel_coding.dat");
	if (!f_tx)
	{
		std::cerr << "error: unable to open output file: "
				  << f_tx << std::endl;
		exit(1);
	}

	for (uint32_T i = 0; i < n_d_bits; i++)
	{
		f_tx << (double)d_bits[i];
	}

	f_tx.close();
}

// modulation
    
// channel simulation
void wrapper_channel(const uint8_T *d_bits, const uint32_T n_d_bits,
					 double *y)
{

}

// receive data from channel
void wrapper_recv(double *d_rx, uint32_T &n_rx)
{
	// just read from file for now
	std::ifstream f_rx;
	std::string str;

	f_rx.open("tx_channel_coding.dat");
	if (!f_rx)
	{
		std::cerr << "error: unable to open input file: "
				  << f_rx << std::endl;
		exit(1);
	}

	f_rx >> str;
	n_rx = str.length();
	for (int i = 0; i < n_rx; i++)
		d_rx[i] = (double)(str[i] - '0');
	f_rx.close();
}

// demodulation
    
// turbo decode
void wrapper_turbo_decoder(const double *y, const uint32_T n_d_bits,
						   int8_T *d, uint32_T &n_d, uint32_T n_iters, uint32_T interleaver_size)
{
	uint32_T n_gens = 2;
	uint32_T gens[2] = {013, 015};
	uint32_T constraint_length = 4;
//	uint32_T n_iters = 1;
    Turbo tb(gens, n_gens, constraint_length, interleaver_size, n_iters);

//	tb.spec_decoder(y, n_d_bits, d, n_d);
	tb.decoder(y, n_d_bits, d, n_d);
	
	// bit demapping (-1->1, +1->0)
	for (uint32_T i = 0; i < n_d; i++)
	{
		d[i] = (1 - d[i]) / 2;
	}
}

//}
