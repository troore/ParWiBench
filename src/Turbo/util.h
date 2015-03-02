
#ifndef __UTIL_H_
#define __UTIL_H_

#include "lte_phy.h"

extern int num_threads;

void check(char *tx_in_fname, char *rx_out_fname);
int gflop_counter_log_decoder(int n);
double gflop_counter_turbo_decoder(LTE_PHY_PARAMS *lte_phy_params, int n_log_decoder_iters);

#endif
