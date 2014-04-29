
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "turbo.h"
#include "types.h"
#include "defines.h"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
	for (uint32_T k = 0; k < TURBO_INT_K_TABLE_SIZE/*1*/; k++)
	{
		// turbo encode input
		uint32_T n_c_bits = TURBO_INT_K_TABLE[TURBO_INT_K_TABLE_SIZE - k - 1];
//		uint32_T n_c_bits = 6144;
		uint8_T c_bits[N_C_BITS];

		// turbo encode output
		uint8_T d_bits[N_D_BITS];
		uint32_T n_d_bits;

		// received data
		double y[N_D_BITS];

		// turbo decode output
		int8_T d[N_C_BITS];
		uint32_T n_d;

		// generate random numbers
		srand((unsigned)time(NULL));
		for (uint32_T i = 0; i < n_c_bits; i++)
		{
			c_bits[i] = (uint8_T)floor(((double)rand() / RAND_MAX + 0.5));
//			c_bits[2 * i] = 1;
//			c_bits[2 * i + 1] = 0;
//			std::cout << (int)c_bits[i];
		}
//		std::cout << std::endl;
		
		// file dump
		std::ofstream tb_tx_out;

		tb_tx_out.open("tx.dat");
		if (!tb_tx_out)
		{
			std::cerr << "error: unable to open input file: "
					  << tb_tx_out << std::endl;

			return -1;
		}
		for (uint32_T i = 0; i < n_c_bits; i++)
		{
			tb_tx_out << (int)(c_bits[i]);
		}
		tb_tx_out.close();

		// turbo encode

		uint32_T n_gens = 2;
		uint32_T gens[2] = {013, 015};
		uint32_T constraint_length = 4;
		uint32_T n_iters = 1;
		Turbo tb(gens, n_gens, constraint_length, n_c_bits, n_iters);
	
//		tb.spec_encoder(c_bits, n_c_bits, d_bits, n_d_bits);
		tb.encoder(c_bits, n_c_bits, d_bits, n_d_bits);


//    std::cout << n_d_bits << std::endl;

		// modulation
    
		// channel

		for (uint32_T i = 0; i < n_d_bits; i++)
		{
//			std::cout << (int)d_bits[i];
			y[i] = (double)(1 - 2 * d_bits[i]);
//			y[i] = (double)d_bits[i];
		}
//		std::cout << std::endl;
    
		// demodulation
    
		// turbo decode
//		tb.spec_decoder(y, n_d_bits, d, n_d);
		tb.decoder(y, n_d_bits, d, n_d);

		// bit demapping (-1->1, +1->0)
		uint32_T n_err = 0;
		for (uint32_T i = 0; i < n_d; i++)
		{
			d[i] = (1 - d[i]) / 2;
			n_err += (d[i] != c_bits[i]);
		}
		std::cout << "interleaver_size=" << n_c_bits << std::endl;
		std::cout << n_err << std::endl;

		// file dump
		std::ofstream tb_rx_out;

		tb_rx_out.open("rx.dat");
		if (!tb_rx_out)
		{
			std::cerr << "error: unable to open input file: "
					  << tb_rx_out << std::endl;

			return -1;
		}
		for (uint32_T i = 0; i < n_d; i++)
		{
			tb_rx_out << (int)(d[i]);
		}
		tb_rx_out.close();
	}

    return 0;
}
