#!/bin/sh
# Creating the isolation-forest
itree_name=1
itree_num=1
Dataset=$1 #name of the dataset
t=$2 #max. no. of itrees
#Create a directory
if [!-d ./iTrees]; then
    mkdir -p ./iTrees;
fi;

START_TIME=`date +%s`
while [ $itree_num -le $t ]
do
     
	     echo `g++ itree.cpp`
	     echo `./a.out $itree_name $itree_num $Dataset`
     
     itree_name=`expr $itree_name + 1`
     itree_num=`expr $itree_num + 1`     
done
END_TIME=`date +%s`
RUN_TIME=$((END_TIME-START_TIME))
echo "" > Time_iForest
echo "iForest creation time: "$RUN_TIME" seconds ">>Time_iForest

