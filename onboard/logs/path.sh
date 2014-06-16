#!/bin/bash

if [ "x$1" = "x" ] ; then
	echo "Usage: $0 <log file with position column>"
	exit 1
fi

FILE=$1
TMP=tmp.dat
TMP2=tmp2.dat
TMP3=tmp3.dat

grep -v "register" "$FILE" | grep position >$TMP
grep -v "register" "$FILE" | grep rotation >$TMP2

paste "$TMP" "$TMP2" >"$TMP3"

if [ "x$2" = "x3" ] ; then
	gnuplot -p -e "set view equal xyz; splot '$TMP3' every 10 using 3:4:5:(0.1*cos(\$10)):(0.1*sin(\$10)):(0) with vectors t 'path'"
else
	gnuplot -p -e "set size ratio -1; plot '$TMP3' every 10 using 3:4:(0.1*cos(\$10)):(0.1*sin(\$10)) with vectors t 'path'"
fi
