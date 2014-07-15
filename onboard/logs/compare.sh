#!/bin/bash

cat - >tmp.dat

grep "(dronetowp)" tmp.dat >pred.dat
grep "(dronetowr)" tmp.dat >result.dat
grep "(metadata)" tmp.dat >meta.dat

FIELD=$1

gnuplot -p -e 'plot "pred.dat" using ($13):'$FIELD' w lp t "pred", "result.dat" using ($13):($'$FIELD'-0) w lp t "result", "meta.dat" using 7:($8/10) t "status", "meta.dat" using 7:9 t "EKF" '
