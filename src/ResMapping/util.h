
#ifndef __UTIL_H_
#define __UTIL_H_

#include "lte_phy.h"

extern int num_threads;

void check(char *tx_in_fname_real, char *tx_in_fname_imag,
		   char *rx_out_fname_real, char *rx_out_fname_imag);
double gflop_counter(LTE_PHY_PARAMS *lte_phy_params);

#endif
