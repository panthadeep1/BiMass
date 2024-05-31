#!/bin/sh
var=1
path="./subsample/"
while [ $var -le $1 ]
do
    echo `g++ pre_proc_data.cpp`
    echo `./a.out $path$var`
    var=`expr $var + 1`
done
