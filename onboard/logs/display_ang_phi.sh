grep -v register $1 | grep "[^_]rotation" > phi-ref.dat
grep -v register $1 | grep "ekf_rotation" > phi.dat

gnuplot -p -e "plot 'phi.dat' using 1:3 w l t 'ekf phi', 'phi-ref.dat' using 1:3 w l t 'imu phi'"