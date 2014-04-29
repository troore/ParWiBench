#!/usr/bin/env python

def turbo_constituent_encoder(u, y_p):
	constraint_len = 3
	s_reg = [0, 0, 0]
	g_array = [1, 0, 1]
	fb_array = [0, 1, 1]

	for i in range(len(u)):
		j = constraint_len - 1
		for j in range(constraint_len - 1, 0, -1):
			s_reg[j] = s_reg[j - 1]

		fb_bit = 0
		for j in range(constraint_len):
			fb_bit = fb_bit + s_reg[j] * fb_array[j] 
		s_reg[0] = (fb_bit + u[i]) % 2

		y_p[i] = 0
		for j in range(constraint_len):
			y_p[i] = y_p[i] + (s_reg[j] * g_array[j])

		y_p[i] = y_p[i] % 2
		
def turbo_internal_interleaver(u, u_prime):
	u_prime_prime = [u[2], u[3], u[0], u[4], u[1], u[5], u[6]]
	for i in range(len(u)):
		u_prime[i] = u_prime_prime[i]

def turbo_encode(u, y):
	n_u_bits = len(u)
	y_1p = [None] * n_u_bits
	u_prime = [None] * n_u_bits
	y_2p = [None] * n_u_bits

	turbo_constituent_encoder(u, y_1p)
	turbo_internal_interleaver(u, u_prime)
	turbo_constituent_encoder(u_prime, y_2p)

	print y_1p
	print u_prime
	print y_2p

	n_u_bits = len(u)
	for i in range(len(u)):
		y[i] = u[i]
		y[n_u_bits + i] = y_1p[i]
		y[2 * n_u_bits + i] = y_2p[i]
	
	return y


if __name__ == "__main__":
	u = [1, 0, 1, 0, 1, 0, 0]
	y = [None] * (3 * len(u))
#	print len(c_bits)
#	N_branch_bits = N_c_bits + 4
	turbo_encode(u, y)
	print y
