#ifndef __PCCC_WRAPPER_H_
#define __PCCC_WRAPPER_H_

extern "C" {
	void wrapper_turbo_encoder(const uint8_T *c_bits, const uint32_T n_c_bits,
							   uint8_T *d_bits, uint32_T &n_d_bits);
	void wrapper_send(uint8_T *d_bits, uint32_T n_d_bits);

	void wrapper_channel(const uint8_T *d_bits, const uint32_T n_d_bits,
						 double *y);
	void wrapper_turbo_decoder(const double *y, const uint32_T n_d_bits,
							   int8_T *d, uint32_T &n_d, uint32_T n_iter, uint32_T interleaver_size);
	void wrapper_recv(double *d_rx, uint32_T &n_rx);
}

#endif
