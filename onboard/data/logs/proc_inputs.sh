#!/bin/bash

# Usage: $0 filename 
# Creates separate data file for each specified input.

FILE="$1"

for t in rollE pitchE yawE psi phi theta battery altitude ; do
	grep "$t " "$FILE" >$t.dat
done


