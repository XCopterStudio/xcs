#!/bin/bash

if [ "x$2" = "x" ] ; then
	echo "Usage: $0 <log-file> <video-delay-seconds>"
	exit 1
fi

FILE=$1
DELAY=$2
TMP=`mktemp`

cp "$FILE" "$FILE~"
sed -i '1,100s/^$/register videoTime TIME_LOC double\n/' $FILE

awk -v OFS="\t" -v delay=$DELAY '{lastTime = $1; print }
$2 ~/internalTimeImu/ { offset = $3 - $1 }
$2 ~/video/ && $1 !~ /register/ { print lastTime, "videoTime", lastTime + offset - delay}' $FILE >$TMP

mv $TMP $FILE

