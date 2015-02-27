#!/bin/sh

./Turbo_omp.out -f 4 -n 1 > log
for i in 2 4 6 8 12 16 20 22 24
do
	./Turbo_omp.out -f 4 -n $i >> log
done

sed -n '/gathering/p' log
sed -n '/PACKAGE_ENERGY:PACKAGE0/p' log
