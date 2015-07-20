#!/bin/bash

scaling_available_frequencies="1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2.0"
#scaling_available_frequencies="1.2 1.3"

echo "#	perf	energy	power" > $2.dat

for i in $scaling_available_frequencies
do
freq=$i
for j in {0..23}
do
cpufreq-set -c $j -f ${i}GHz
done
./$1 2048 16 > log
perf=`sed -n '/Stopping/p' log | awk '{print $4}'`
energy_power=`sed -n '/PACKAGE_ENERGY:PACKAGE0/p' log | awk '{print $2, $6}'`
echo $freq $perf $energy_power >> $2.dat
done

for j in {0..23}
do
cpufreq-set -c $j -f 1.2GHz
done
