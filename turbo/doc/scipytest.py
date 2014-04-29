#!/usr/bin/env python

from scipy import *
from pylab import *
from numpy import array
import math

def newLine():
    print "First line."

def plotfft():
    a = zeros(1000)
    a[:100] = 1
    b = fft(a)
 #   plot(abs(b))
 #   show()

    real = array([1, 2, 3, 4])
    img = array([4, 3, 2, 1])
    c = real + 1j * img 
    print c
    d = fft(c)
    print d
    print abs(d)    
    plot(abs(d))
    show()
 
    
if __name__ == "__main__":
    newLine()
    plotfft()
        
