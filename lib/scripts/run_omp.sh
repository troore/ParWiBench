#!/bin/sh

./RM_omp.out -f 4 -n 1 > log
for i in `seq {2,24}`
do
	./RM_omp.out -f 4 -n $i >> log
done

sed -n '/gathering/p' log
sed -n '/PACKAGE_ENERGY:PACKAGE0/p' log
