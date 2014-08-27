#!/bin/bash

if [ "x$1" = "x" ] ; then
	echo "Usage: $0 <log-file>"
	exit 1
fi

FILE=$1
TMP=`mktemp`

awk -v OFS="\t" '
$2 ~/(velocity|acceleration|gyro|magneto)/ && $1 !~ /register/ { print $1, $2, -$4, $3, $5 ; next }
{ print }' $1 >$TMP

mv $TMP $FILE

