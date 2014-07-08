#!/bin/bash

if [ "x$1" = "x" ] ; then
	echo "Usage: $0 <log file with position column>"
	exit 1
fi

FILE=$1
TMP=tmp.dat
TMP2=tmp2.dat
TMP3=tmp3.dat
POSE_SAMPLE=5

grep -v "register" "$FILE" | grep position >$TMP
grep -v "register" "$FILE" | grep rotation >$TMP2

paste "$TMP" "$TMP2" >"$TMP3"
#TERM_BEG="set term pdfcairo size 10,8; set output 'path$2.pdf'; set grid;"
#TERM_END="unset output"
LABELS="set xlabel 'x [m]'; set ylabel 'y [m]'"
LABELS3="set zlabel 'z [m]'"

# $8   phi
# $9   theta
# $10  psi

if [ "x$2" = "x3" ] ; then
	LEN=0.2
	AMP=5
	FORWARD="($LEN*sin(\$10)*sin($AMP*\$9)):($LEN*cos(\$10)*cos($AMP*\$9)):($LEN*sin($AMP*\$9))"
	RIGHT="($LEN*cos(\$10)*cos($AMP*\$8) + $LEN*sin(\$10)*sin($AMP*\$9)*sin($AMP*\$8)):(-$LEN*sin(\$10)*cos($AMP*\$8) + $LEN*cos(\$10)*sin($AMP*\$9)*sin($AMP*\$8)):(-$LEN*cos($AMP*\$9)*sin($AMP*\$8))"
	TOP="($LEN*cos(\$10)*sin($AMP*\$8) - $LEN*sin(\$10)*sin($AMP*\$9)*cos($AMP*\$8)):(-$LEN*sin(\$10)*sin($AMP*\$8) - $LEN*cos(\$10)*sin($AMP*\$9)*cos($AMP*\$8)):($LEN*cos($AMP*\$9)*cos($AMP*\$8))"
	gnuplot -p -e "$TERM_BEG set view equal xyz; $LABELS; $LABELS3;	splot\
	   '$TMP3' every $POSE_SAMPLE using 3:4:5:$FORWARD with vectors t 'y',\
   	   '$TMP3' every $POSE_SAMPLE using 3:4:5:$RIGHT with vectors t 'x',\
   	   '$TMP3' every $POSE_SAMPLE using 3:4:5:$TOP with vectors t 'z',\
	   '$TMP3' every ::1::1 using 3:4:5 pt 7 t 'start',\
	   '$TMP3' using 3:4:5 with lines t 'path';\
	   $TERM_END"
else
	gnuplot -p -e "$TERM_BEG set size ratio -1; $LABELS; plot '$TMP3' every $POSE_SAMPLE using 3:4:(0.1*sin(\$10)):(0.1*cos(\$10)) with vectors t 'psi', '$TMP3' using 3:4 with lines t 'path'; $TERM_END"
fi
