from ctypes import *
import time
import numpy
import matplotlib.pyplot as plt
import math

#time.sleep(30)

libturbo = CDLL("./libturbo.so")

N_C_BITS = 6144
N_D_BITS = ((N_C_BITS + 4) * 3)

# received data from channel
d_rx_ctype = (c_double * N_D_BITS)()
n_d_rx_ctype = c_uint()
    
# after adding noise
y_ctype = (c_double * N_D_BITS)()

# turbo decode output
d_ctype = (c_byte * N_C_BITS)()
n_d_ctype = c_uint()


# simulation parameters
# type: numpy.ndarray
EbN0_dB = numpy.arange(0, 5, 0.1)
snr_len = EbN0_dB.shape[0]
# coding rate (non punctured PCCC)
R = (1.0 / 3.0)
# coding bit energy
Ec = 1
# tyep: numpy.ndarray
sigma2 = (0.5 * Ec / R) * 10 ** (-EbN0_dB / 10)
# upper bound of number of iterations
nb_iter = 10
# BER
BER = numpy.zeros((snr_len, nb_iter))

c_bits = [int] * N_C_BITS
f_tx = open("tx.dat", "r")

text = f_tx.readline()
#    print text
# delete '\n'
n_c_bits = len(text)
#print n_c_bits

for i in range(n_c_bits):
    c_bits[i] = int(text[i])
#        print c_bits[i],
#	print
f_tx.close()

for en in range(1, snr_len + 1):
    # receive data from channel
    libturbo.wrapper_recv(d_rx_ctype, byref(n_d_rx_ctype))
#    print n_d_rx_ctype.value
#    for i in range(n_d_rx_ctype.value):
#        print d_rx_ctype[i],
#    print
 
    # add noise
    AWGN_random = numpy.random.randn(n_d_rx_ctype.value)
    for i in range(n_d_rx_ctype.value):
        y_ctype[i] = (1 - 2 * d_rx_ctype[i]) + math.sqrt(sigma2[en - 1]) * AWGN_random[i]
#        y_ctype[i] = (1 - 2 * d_rx_ctype[i])
#        y_ctype[i] = (-2 / sigma2[en - 1]) * y_ctype[i]

    # turbo decode
    for n in range(1, nb_iter + 1):
        libturbo.wrapper_turbo_decoder(y_ctype, n_d_rx_ctype, d_ctype, byref(n_d_ctype), c_uint(n), c_uint(n_c_bits))
        for i in range(n_d_ctype.value):
            if d_ctype[i] != c_bits[i]:
                BER[en - 1, n - 1] = BER[en - 1, n - 1] + 1
        print "BER[%d,%d] is %d" %(en, n, BER[en - 1, n - 1]),
#            print d_ctype[i],
#        print
        BER[en - 1, n - 1] = (BER[en - 1, n - 1] / n_c_bits)
    print

        # write output of turbo decoder to file for test
#        f_rx = open("rx.dat", "w")
#        for i in range(n_d_ctype.value):
#            f_rx.write(str(d_ctype[i]))
#        f_rx.close()

plt.figure(figsize = (8, 4))
plt.semilogy(EbN0_dB, BER, 'o-')
plt.grid(True)
plt.xlabel('Eb/N0 [dB]')
plt.ylabel('BER')
plt.show()
