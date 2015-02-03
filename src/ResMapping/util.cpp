
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "check/check.h"
#include "GeneralFunc.h"

int num_threads;

void check()
{
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;

//	strcpy(tx_in_fname, "../testsuite/SubCarrierMapInputReal");
//	strcpy(rx_out_fname, "../testsuite/testLSCELSEqOutputReal");
	strcpy(tx_in_fname, "/root/ResMapping/testsuite/SubCarrierMapInputReal");
	strcpy(rx_out_fname, "/root/ResMapping/testsuite/testLSCELSEqOutputReal");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	strcpy(tx_in_fname, "/root/ResMapping/testsuite/SubCarrierMapInputImag");
	strcpy(rx_out_fname, "/root/ResMapping/testsuite/testLSCELSEqOutputImag");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

double gflop_counter(LTE_PHY_PARAMS *lte_phy_params)
{
	
	int cnter = 0;

	return 1.0e-9 * cnter;
}
