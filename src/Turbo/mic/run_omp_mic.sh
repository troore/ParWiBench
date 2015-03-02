#!/bin/sh
EXE=$1
#FS := $1
#N := $2

$EXE -f 4 -n 1 > log1

for((i=4;i<=4;i++))
do
#for((j=4;j<=8;j+=4))
for((j=4;j<=236;j+=4))
do
echo $i $j
$EXE -f $i -n $j >> log1
done
done

sed -n '6p' log1 > log2
#for((i=6+8;i<=6+2*8;i+=8))
for((i=6+8;i<=6+59*8;i+=8))
do
echo $i
sed -n ''"$i"'p' log1 >> log2
done
