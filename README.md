
### Usage

Enter source directory:

	cd src

Enter module you are interested in, i.e., Modulation:

	cd Modulation

If you want to test the transmitter, build with

	make TX=1

Then you could run the transmitter (a.k.a., TX) use 3 arguments:

	./ModDemod.out 0 2 2 2

You could check what the input/output files in the test file <code>ModMain.cpp</code>. In the similar way, build and run the receiver (RX) like:

	make
	./ModDemod.out 0 2 2 2

I will explain to you what the arguments and the input/outfile mean later.

### Code References

[1] [WiBench](http://wibench.eecs.umich.edu)

[2] [OpenLTE](http://openlte.sourceforge.net/)

[3] [OpenAirInterface](https://twiki.eurecom.fr/twiki/bin/view/OpenAirInterface/WebHome)

[4] MATLAB R2013b LTE Toolkit

[5] [itpp library](itpp.sourceforge.net)

[6] [libLTE](https://github.com/libLTE/libLTE)

[7] [LTE Uplink Receiver PHY Benchmark](http://sourceforge.net/projects/lte-benchmark/)
