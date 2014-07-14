#!/bin/bash

cat - >tmp.dat

grep "(dronetowp)" tmp.dat >pred.dat
grep "(dronetowr)" tmp.dat >result.dat

FIELD=$1

gnuplot -p -e 'plot "pred.dat" using ($13):'$FIELD' t "pred", "result.dat" using ($13):'$FIELD'  t "result"'
