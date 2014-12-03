
import math

# trellis_mapper:
# an entry in trellis is:
# {"s(k-1)s(k)": [c(k, 1), c(k, 2), u(k)]}
trellis = {
		"11": [-1, -1, -1],
		"23": [-1, -1, -1],
		"34": [-1, 1, -1],
		"42": [-1, 1, -1],
		"13": [1, 1, 1],
		"21": [1, 1, 1],
		"32": [1, -1, 1],
		"44": [1, -1, 1]
		}

Lc = 1.0

def interleaver(u, u_prime):
	u_prime_prime = [u[0], u[3], u[5], u[1], u[2], u[4], u[6], u[7]]
	for i in range(len(u)):
		u_prime[i] = u_prime_prime[i]

def deinterleaver(u, u_prime):
	u_prime_prime = [u[0], u[3], u[4], u[1], u[5], u[2], u[6], u[7]]
	for i in range(len(u)):
		u_prime[i] = u_prime_prime[i]

def get_gamma_e(gamma_e, y_p, k):
	for i in range(1, 5):
		for j in range(1, 5):
			id = str(i) + str(j)
			if id in trellis:
				gamma_e[i][j] = math.exp(Lc * 0.5 * y_p[k] * trellis[id][1])

def get_gamma(gamma, y_s, y_p, k, L_apriori):
	for i in range(1, 5):
		for j in range(1, 5):
			id = str(i) + str(j)
			if id in trellis:
				gamma[i][j] = math.exp(0.5 * L_apriori * trellis[id][0] +
						Lc * 0.5 * y_s[k] * trellis[id][0]) * math.exp(Lc * 0.5 * y_p[k] * trellis[id][1])

def get_alpha(alpha_k, alpha_k_minus, gamma_k):
	l = len(alpha_k)
	for s in range(1, l):
		numerator = 0
		for s_prime in range(1, l):
			numerator = numerator + alpha_k_minus[s_prime] * gamma_k[s_prime][s]

		alpha_k[s] = numerator


def get_beta(beta_k_minus, beta_k, gamma_k):
	l = len(beta_k_minus)
	for s_prime in range(1, l):
		numerator = 0
		for s in range(1, l):
			numerator = numerator + beta_k[s] * gamma_k[s_prime][s]

		beta_k_minus[s_prime] = numerator

def get_denominator(alpha_k_minus, gamma_k):
	l = len(alpha_k_minus)
	denominator = 0
	for i in range(1, l):
		for j in range(1, l):
			denominator = denominator + alpha_k_minus[j] * gamma_k[j][i]

	return denominator

def normalize(a, denominator):
	lk = len(a)
	ls = len(a[0])
	for i in range(lk):
		for j in range(1, ls):
			a[i][j] = a[i][j] / denominator[i]


def get_extrinsic(alpha_k_minus, beta_k, gamma_e_k):
	l = len(alpha_k_minus)
	numerator = 0
	denumerator = 0
	for s_prime in range(1, l):
		for s in range(1, l):
			id = str(s_prime) + str(s)
			if id in trellis:
				if trellis[id][2] == 1:
					numerator = numerator + alpha_k_minus[s_prime] * beta_k[s] * gamma_e_k[s_prime][s]
				else:
					denumerator = denumerator + alpha_k_minus[s_prime] * beta_k[s] * gamma_e_k[s_prime][s]

	# FIXME: What if denumerator equals to zero?
	if denumerator == 0:
		return 1
	else:
		return math.log((numerator / denumerator))

def turbo_decode(y):
	u_len = len(y) / 3
	# received message with noise
	recv_sys1 = [float] * (u_len + 1)
	recv_sys2 = [float] * (u_len + 1)
	recv_prt1 = [float] * (u_len + 1)
	recv_prt2 = [float] * (u_len + 1)
	# final output
	L = [float] * (u_len + 1)

	for i in range(0, u_len):
		recv_sys1[i + 1] = y[3 * i]
		recv_prt1[i + 1] = y[3 * i + 1]
		recv_prt2[i + 1] = y[3 * i + 2]
	
	interleaver(recv_sys1, recv_sys2)

	gamma = [[[0 for i in range(5)] for j in range(5)] for k in range(u_len + 1)]
	gamma_e = [[[0 for i in range(5)] for j in range(5)] for k in range(u_len + 1)]
	alpha = [[0 for i in range(5)] for j in range(u_len + 1)]
	beta = [[0 for i in range(5)] for j in range(u_len + 1)]
	L_apriori = [0] * (u_len + 1)
	L_extrinsic = [0] * (u_len + 1)
	L = [0] * (u_len + 1)

	# initialization
	for i in range(1, 5):
		if i == 1:
			alpha[0][i] = 1
			beta[u_len][i] = 1
		else:
			alpha[0][i] = 0
			beta[u_len][i] = 0

	n_iter = 4
	# iterative decoding
	for i in range(n_iter):
		denominator = [0 for i in range(u_len + 1)]
		denominator[0] = 1
		# 1st decoder
		for k in range(1, u_len + 1):
			get_gamma_e(gamma_e[k], recv_prt1, k)
			get_gamma(gamma[k], recv_sys1, recv_prt1, k, L_apriori[k])

	#	for s_prime in range(1, 5):
	#		for s in range(1, 5):
	#			id = str(s_prime) + str(s)
	#			if id in trellis:
	#				print id
	#				for k in range(1, u_len + 1):
	#					print "%.3f " % gamma[k][s_prime][s],
	#				print

		for k in range(1, u_len + 1):
			get_alpha(alpha[k], alpha[k - 1], gamma[k])
		
	#	for k in range(1, u_len + 1):
	#		for s in range(1, 5):
	#			print "%.3f " % alpha[k][s],
	#		print

		for k in range(1, u_len + 1):
			denominator[k] = get_denominator(alpha[k - 1], gamma[k])
		normalize(alpha, denominator)
		for k in range(u_len, 1, -1):
			get_beta(beta[k - 1], beta[k], gamma[k])

	#	for k in range(1, u_len + 1):
	#		for s in range(1, 5):
	#			print "%.3f " % beta[k][s],
	#		print

		normalize(beta, denominator)
		for k in range(1, u_len + 1):
			L_extrinsic[k] = get_extrinsic(alpha[k - 1], beta[k], gamma_e[k])

		# decision
		for k in range(1, u_len + 1):
			L[k] = L_apriori[k] + Lc * recv_sys1[k] + L_extrinsic[k]
		print L
		for k in range(1, u_len + 1):
			print "%.3f" % L[k],
		print 

		# interleaver
		interleaver(L_extrinsic, L_apriori)

		# 2nd decoder
		for j in range(u_len + 1):
			denominator[i] = 0
		denominator[0] = 1
		for k in range(1, u_len + 1):
			get_gamma_e(gamma_e[k], recv_prt2, k)
			get_gamma(gamma[k], recv_sys2, recv_prt2, k, L_apriori[k])
			denominator[k] = get_denominator(alpha[k - 1], gamma[k])
		for k in range(1, u_len + 1):
			get_alpha(alpha[k], alpha[k - 1], gamma[k])
		normalize(alpha, denominator)
		for k in range(u_len, 1, -1):
			get_beta(beta[k - 1], beta[k], gamma[k])
		normalize(beta, denominator)
		for k in range(1, u_len + 1):
			L_extrinsic[k] = get_extrinsic(alpha[k - 1], beta[k], gamma_e[k])

		# decision
		for k in range(1, u_len + 1):
			L[k] = L_apriori[k] + Lc * recv_sys2[k] + L_extrinsic[k]
		print L
		for k in range(1, u_len + 1):
			print "%.3f" % L[k],
		print

		# deinterleaver
		deinterleaver(L_extrinsic, L_apriori)


#	print recv_sys
#	print recv_prt1
#	print recv_prt2

if __name__ == "__main__":
	y = [2, -5, 6, 1, 2, -1, 3, 
			-1,	2, -2, -2, -2, 2, 1,
			-5, -4, -2, 5, -5, -1, -6]
	turbo_decode(y)
#	gamma = [[[0 for i in range(2)] for j in range(2)] for k in range(3)]
#	gamma[0][0][0] = 1
#	print gamma
#	print gamma[0]
#	gamma = [[0] * 2 for j in range(2)]
#	gamma[1][1] = 5
#	print gamma	
