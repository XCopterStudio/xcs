#!/bin/bash

# Usage: $0 filename fieldname
# Draws histogram of timestamp differences in given input.

TMP=tmp.dat
TMP2=tmp2.dat

grep "$2" "$1" >$TMP
awk '{print $4,$2,$4-prev; prev=$4}' <$TMP >$TMP2

#gnuplot -p -e "set xrange [0:100]; binwidth=1;bin(x,width)=width*floor(x/width);plot \"$TMP2\" using (bin(\$3,binwidth)):(1.0) smooth freq with boxes"
gnuplot -p -e "set xrange [0:100]; binwidth=1;bin(x,width)=width*floor(x/width);plot \"$TMP2\" using 3:(1.0) smooth freq with boxes"
