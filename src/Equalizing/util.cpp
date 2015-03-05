
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "check/check.h"
#include "GeneralFunc.h"

int num_threads;

void check(char *tx_in_fname_real, char *tx_in_fname_imag, char *rx_out_fname_real, char *rx_out_fname_imag)
{
	int err_n;

	err_n = check_float(tx_in_fname_real, rx_out_fname_real);
	printf("%d\n", err_n);
	
	err_n = check_float(tx_in_fname_imag, rx_out_fname_imag);
	printf("%d\n", err_n);
}

inline static int num_flops_matrixprod(int d1, int d2, int d3)
{
	return (6 * d1 * d2 * d3);
}

inline static int num_flops_matrixinv(int d)
{
	return (28 * d * d + 16 * d * d * (d - 1));
}

double gflop_counter(LTE_PHY_PARAMS *lte_phy_params)
{
	int n_dft = lte_phy_params->N_dft_sz;
	int n_tx_ant = lte_phy_params->N_tx_ant;
	int n_rx_ant = lte_phy_params->N_rx_ant;
	int n_symbs_sf = LTE_PHY_N_SYMB_PER_SUBFR;
	
	int cnter = 0;

	cnter = cnter + num_flops_matrixprod(n_tx_ant, 2, n_tx_ant) + num_flops_matrixprod(n_tx_ant, 2, n_rx_ant)
		+ num_flops_matrixprod(n_tx_ant, n_tx_ant, n_rx_ant) + num_flops_matrixinv(n_tx_ant);
	cnter = cnter + num_flops_matrixprod(n_tx_ant, n_rx_ant, n_tx_ant) + num_flops_matrixinv(n_tx_ant)
		+ (n_symbs_sf - 2) * (num_flops_matrixprod(n_tx_ant, n_rx_ant, 1) + num_flops_matrixprod(n_tx_ant, n_tx_ant, 1));

	cnter = cnter * n_dft;

	return 1.0e-9 * cnter;
}
