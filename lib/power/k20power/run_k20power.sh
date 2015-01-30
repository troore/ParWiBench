#!/bin/sh

for i in $(seq 0 4)
do
	echo $i
	../../../lib/power/k20power/K20power $1 $i >> log
done

sed -n '/Elapsed/p' log >&1
