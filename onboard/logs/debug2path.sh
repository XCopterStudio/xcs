#!/bin/bash

if [ "x$1" = "x" ] ; then
	echo "Usage: $0 <keyword>"
	exit 1
fi

KEYWORD=$1


grep "PTAM ($KEYWORD)" - | awk -v OFS="\t" '{print $13, "position", $7, $8, $9, "\n" $13, "rotation", $10, $11, $12}'
