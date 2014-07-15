#!/bin/bash

cat - >tmp.dat

grep "(dronetowp)" tmp.dat >pred.dat
grep "(dronetowr)" tmp.dat >result.dat
grep "(metadata)" tmp.dat >meta.dat
grep "(cam_update)" tmp.dat >cam.dat
grep "(cam_predict)" tmp.dat >cam_pred.dat
grep "(imu)" tmp.dat > imu.dat

FIELD=$1

gnuplot -p -e 'plot "pred.dat" using ($13):'$FIELD' w lp t "pred", "result.dat" using ($13):($'$FIELD'-0) w lp t "result", "meta.dat" using 7:($8/10) t "status", "meta.dat" using 7:9 t "EKF", "cam.dat" using ($13):'$FIELD' w lp t "cam", "imu.dat" using ($13):'$FIELD' w lp t "ekf_imu", "cam_pred.dat" using ($13):'$FIELD' w lp t "cam_pred" ; pause -1'

