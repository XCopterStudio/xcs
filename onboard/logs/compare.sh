#!/bin/bash

cat - >tmp.dat

grep "(dronetowp)" tmp.dat >pred.dat
grep "(dronetowr)" tmp.dat >result.dat

gnuplot -p -e 'plot "pred.dat" using ($13):9  t "pred", "result.dat" using ($13):9  t "result"'
