
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "check/check.h"

int num_threads;

void check(char *tx_in_fname, char *rx_out_fname)
{
	int err_n;
	
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

double gflop_counter(LTE_PHY_PARAMS *lte_phy_params)
{
	int cnter = 0;
	int mod_table_len, bits_per_samp;

	switch(lte_phy_params->mod_type)
	{
	case LTE_BPSK:
		bits_per_samp = BPSK_BITS_PER_SAMP;
		mod_table_len = BPSK_TABLE_LEN;
		break;
	case LTE_QPSK:
		bits_per_samp = QPSK_BITS_PER_SAMP;
		mod_table_len = QPSK_TABLE_LEN;
		break;
	case LTE_QAM16:
		bits_per_samp = QAM16_BITS_PER_SAMP;
		mod_table_len = QAM16_TABLE_LEN;
		break;
	case LTE_QAM64:
		bits_per_samp = QAM64_BITS_PER_SAMP;
		mod_table_len = QAM64_TABLE_LEN;
		break;
	default:
		printf("Invalid modulation type!\n");
		// TODO: Exception processing
		break;
	}

	cnter += mod_table_len * 5;
	cnter += bits_per_samp * mod_table_len;

	cnter *= lte_phy_params->demod_in_buf_sz;

	return 1.0e-9 * cnter;
}

