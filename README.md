
### mod usage

### turbo usage

**functional test**

	$ cd turbo
	$ make turbo
	$ ./turbo

Then check if "tx.dat" matches "rx.dat", where "tx.dat" is generated data to encode and send, and "rx.dat" is data received and decoded.

**plot**

	$ cd turbo
	$ make libturbo
	$ python tx.py
	$ python rx.py

Where "tx.py" is script controlling data generating and sending, and "rx.py" is that controlling data receiving and decoding, then plotting.
