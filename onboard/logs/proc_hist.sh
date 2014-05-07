#!/bin/bash

# Usage: $0 filename fieldname
# Draws histogram of timestamp differences in given input.

TMP2=tmp2.dat

grep "$2" "$1" | awk '{print $1,$2,$1-prev; prev=$1}' >$TMP2

gnuplot -p <<EOD
minFreq = 10;
maxFreq = 400;
set xrange [minFreq:maxFreq];
set x2range [1000/maxFreq:1000/minFreq];

set xtics nomirror
set x2tics

set xlabel "Frequency [Hz]"
set ylabel "Hits"
set x2label "Difference [ms]"

binwidth=1;
bin(x,width)=width*floor(x/width);
plot "$TMP2" using (bin(1000/\$3,binwidth)):(1.0) smooth freq with boxes t "$2 (f)", "$TMP2" using (bin(\$3,binwidth)):(1.0) axes x2y1 smooth freq with boxes t "$2 (diff)"
EOD

#gnuplot -p -e "binwidth=1;bin(x,width)=width*floor(x/width);plot \"$TMP2\" using (bin(1000/\$3,binwidth)):(1.0) smooth freq with boxes t \"frequency [Hz]\", \"$TMP2\" using (bin(\$3,binwidth)):(1.0) smooth freq with boxes t \"difference [ms]\""
