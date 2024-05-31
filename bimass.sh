#!/bin/sh
# Creating the isolation-forest
time_BiMass=""
memory_BiMass=""
iNodes=""
output_BiMass=""
echo "$time_BiMass" > Time_BiMass
echo "$memory_BiMass" > Memory_BiMass
echo "$iNodes" > iNodes #Nodes affected
echo "$output_BiMass" > output_BiMass

dataset=$1 #name of the base dataset
t=$2 #no. of the inserted files

var=1

#START_TIME=`date +%s`
while [ $var -le $t ]
do
     
	if [ $var -eq 1 ]
        then
	     echo `g++ bimass.cpp`
	     echo `./a.out $dataset $var`
        else
	     echo `g++ bimass.cpp`
	     echo `./a.out data_matrix $var`
        fi
        var=`expr $var + 1`   
         
done
#END_TIME=`date +%s`
#$RUN_TIME=$((END_TIME-START_TIME))


