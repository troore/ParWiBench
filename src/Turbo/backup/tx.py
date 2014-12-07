from ctypes import *
import time
import numpy
import matplotlib.pyplot as plt
import math

#time.sleep(30)

libturbo = CDLL("./libturbo.so")

#TenIntegers = c_int * 10
#ii = TenIntegers(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
#ii = TenIntegers()

#for i in range(10):
#    ii[i] = i
    
#n_ii = c_int(10)
#jj = TenIntegers([int] * 10)
#oo = TenIntegers(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#oo = TenIntegers()
#n_oo = c_int()

#so.display_pointer(ii, n_ii, oo, byref(n_oo))

#print repr(n_oo.value)

N_C_BITS = 6144
N_D_BITS = ((N_C_BITS + 4) * 3)

# turbo encode input
n_c_bits = 6144
c_bits_ctype = (c_ubyte * N_C_BITS)()
n_c_bits_ctype= c_uint(n_c_bits)

# turbo encode output
d_bits_ctype = (c_ubyte * N_D_BITS)()
n_d_bits_ctype = c_uint()

f_tx = open("tx.dat", "w")
# generate random input bits
c_bits = numpy.random.randint(0, 2, n_c_bits)
#    print c_bits

# write input for turbo encoder to file for test

for i in range(n_c_bits):
    f_tx.write(str(c_bits[i]))
    
f_tx.close()

for i in range(n_c_bits):
    c_bits_ctype[i] = c_bits[i]
    
libturbo.wrapper_turbo_encoder(c_bits_ctype, n_c_bits_ctype, d_bits_ctype, byref(n_d_bits_ctype))

libturbo.wrapper_send(d_bits_ctype, n_d_bits_ctype)


