
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "check/check.h"
#include "GeneralFunc.h"

int num_threads;

void check(char *tx_in_fname, char *rx_out_fname)
{
	int err_n;

	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

int gflop_counter_log_decoder(int n)
{
	int cnter = 0;

	// gamma
	cnter += n * N_STATES * ((N_GENS - 1) * 2 + 6);
	// alpha
	cnter += n * N_STATES * 5;
	// beta
	cnter += (n - 1) * N_STATES * 4;
	// extrinsic
	cnter += n * (N_STATES * ((N_GENS - 1) * 2 + 6) + 1);

//	return 1.0e-9 * cnter;
	return cnter;
}

double gflop_counter_turbo_decoder(LTE_PHY_PARAMS *lte_phy_params, int n_log_decoder_iters)
{
	int cnter = 0;
	int data_length;
	int last_block_length;
	int cur_blk_len;
	int n_blocks;

	data_length = lte_phy_params->td_out_buf_sz;
	n_blocks = ((data_length + BLOCK_SIZE - 1) / BLOCK_SIZE);
	if (data_length % BLOCK_SIZE)
	{
		last_block_length = (data_length % BLOCK_SIZE);
	}
	else
	{
		last_block_length = BLOCK_SIZE;
	}

	for (int i = 0; i < n_blocks; i++)
	{
		int cur_blk_n_gflops;
		
		cur_blk_len = (i != (n_blocks - 1)) ? BLOCK_SIZE : last_block_length;
		cur_blk_n_gflops = gflop_counter_log_decoder(cur_blk_len);
		cnter +=  2 * n_log_decoder_iters * cur_blk_n_gflops;

		printf("%d\n", cur_blk_n_gflops);
	}

	return 1.0e-9 * cnter;
}
