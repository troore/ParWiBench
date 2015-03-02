#!/bin/sh
EXE=./ResMapper_omp.out
#FS := $1
#N := $2

$EXE -f 4 -n 1 > log1

for((i=4;i<=4;i++))
do
for((j=4;j<=236;j+=4))
do
echo $i $j
$EXE -f $i -n $j >> log1
done
done

sed -n '4p' log1 > log2
for((i=4+9;i<=4+59*9;i+=9))
do
echo $i
sed -n ''"$i"'p' log1 >> log2
done
