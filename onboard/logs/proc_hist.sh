#!/bin/bash

# Usage: $0 filename fieldname
# Draws histogram of timestamp differences in given input.

TMP=tmp.dat
TMP2=tmp2.dat

cat "$1" > $TMP
awk '{print $1,$2,$1-prev; prev=$1}' <"$1" >$TMP2

#gnuplot -p -e "set xrange [0:100]; binwidth=1;bin(x,width)=width*floor(x/width);plot \"$TMP2\" using (bin(\$3,binwidth)):(1.0) smooth freq with boxes"
gnuplot -p -e "set xrange [0:100]; binwidth=1;bin(x,width)=width*floor(x/width);plot \"$TMP2\" using (bin(1000/\$3,binwidth)):(1.0) smooth freq with boxes"
