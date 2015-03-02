
#ifndef __UTIL_H_
#define __UTIL_H_

#include "lte_phy.h"

extern int num_threads;

void check(char *tx_in_fname, char *rx_out_fname);
double gflop_counter(LTE_PHY_PARAMS *lte_phy_params);

#endif
