#!/bin/bash

FILE="$1"
KEY="$2"
TMP=`mktemp`
pi=$(echo "scale=10; 4*a(1)" | bc -l)

grep "PTAM ($KEY)" "$FILE" | sed "s/^.*PTAM ($KEY): \(.*\)$/\\1/" >$TMP

awk -v OFS="\t" '{ print NR, "position", $1, $2, $3} ' $TMP > "$FILE.pos"
awk -v pi="$pi" -v OFS="\t" '{ print NR, "rotation", $4/180*pi, $5/180*pi, $6/180*pi} ' $TMP > "$FILE.rot"

cat "$FILE.pos" "$FILE.rot"
