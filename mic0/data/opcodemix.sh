#!/bin/bash

filename=$1
instrtype=$2
endlinenum=`nl $1 | grep "dynamic-counts" | awk '{print $1}'`
endlinenum=$(($endlinenum + 1))

# total number of instructions
instrvector=`sed "1,${endlinenum}d" $filename | sed -e '/#/d' | grep $instrtype | awk '{print $2}'`
lnvector=`sed "1,${endlinenum}d" $filename | sed -e '/#/d' | grep $instrtype | awk '{print $3}'`

echo $instrvector

#lnvector="1 2 3 4 5"

instr_counter=0

for i in $lnvector
do
instr_counter=$(($instr_counter + $i))
done

echo $instr_counter
