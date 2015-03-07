#!/bin/sh
#EXE=./ResMapper_omp.out
EXE=$1
NUM=$2
#FS := $1
#N := $2

$EXE $NUM 1 > log1

for((j=4;j<=236;j+=4))
do
$EXE $NUM $j >> log1
done

sed -n '3p' log1 > log2
for((i=3+4;i<=3+59*4;i+=4))
do
echo $i
sed -n ''"$i"'p' log1 >> log2
done
