#!/bin/bash

# Usage: $0 filename fieldname
FILE="$1"

for t in rollE pitchE yawE psi phi theta battery ; do
	grep "$t " "$FILE" >$t.dat
done


