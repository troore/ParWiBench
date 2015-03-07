2015-01-06 Xuechao Wei <troore@gmail.com>

[Function]

Add relatively convenient usage for multiple inputs.

[Bug Fixed]

1.	src/lte_phy.h

	Multiply N_MOD_IN_MAX and N_MOD_OUT_MAX with LTE_PHY_N_ANT_MAX.

2.	src/RateMatching/c/RateMatcher_ntp.cpp

	In SubblockInterleaving and SubblockDeinterleaving, the length of local vectors should be K_pi rather than ((BLOCK + 31) / 32) * 32, which would cause segfault when input length of RxRateMatching grows.

[File Structure]

Move several tables in Turbo into lte_phy.h, as these tables are used to determine the input length at the beginning of the whole pipeline.

[Info]

Please cautiously choose the Read/Write/GeneRandom functions in GeneralFunc.cpp/GeneralFunc.h when using different complex format, especially when switch between paired and separated formats. Specified functioins for these two formats are still not implemented individually, so users must comment one when using another.


2015-01-16 Hibbert <lixibai2@126.com>

[Bug Fixed]

RateM: Offset is added directly on the input arguments (pointers), rather than make it a argument and get start addresses of block in the subblockinterleaver function.


2015-03-07 Xuechao Wei <troore@gmail.com>

1.	src/ResMapping/c/ResMapper_omp.cpp

Move <code>geneDMRS</code> out of <code>SubCarrierMapping</code> to test.cpp.
