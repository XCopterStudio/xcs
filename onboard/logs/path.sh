#!/bin/bash

if [ "x$1" = "x" ] ; then
	echo "Usage: $0 <log file with position column>"
	exit 1
fi

FILE=$1
TMP=tmp.dat
TMP2=tmp2.dat
TMP3=tmp3.dat
POSE_SAMPLE=40

grep -v "register" "$FILE" | grep position >$TMP
grep -v "register" "$FILE" | grep rotation >$TMP2

paste "$TMP" "$TMP2" >"$TMP3"

if [ "x$2" = "x3" ] ; then
	LEN=0.2
	AMP=5
	FORWARD="($LEN*sin(\$10)*sin($AMP*\$9)):($LEN*cos(\$10)*cos($AMP*\$9)):($LEN*sin($AMP*\$9))"
	RIGHT="($LEN*cos(\$10)*cos($AMP*\$8) + $LEN*sin(\$10)*sin($AMP*\$9)*sin($AMP*\$8)):(-$LEN*sin(\$10)*cos($AMP*\$8) + $LEN*cos(\$10)*sin($AMP*\$9)*sin($AMP*\$8)):(-$LEN*cos($AMP*\$9)*sin($AMP*\$8))"
	gnuplot -p -e "set view equal xyz; splot '$TMP3' every $POSE_SAMPLE using 3:4:5:$FORWARD with vectors t 'forward', '$TMP3' every $POSE_SAMPLE using 3:4:5:$RIGHT with vectors t 'right', '$TMP3' using 3:4:5 with lines t 'path'"
else
	gnuplot -p -e "set size ratio -1; plot '$TMP3' every $POSE_SAMPLE using 3:4:(0.1*sin(\$10)):(0.1*cos(\$10)) with vectors t 'psi', '$TMP3' using 3:4 with lines t 'path'"
fi
