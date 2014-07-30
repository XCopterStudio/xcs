grep -v register $1 | grep ekf.velocity > vel.dat

gnuplot -p -e "plot 'vel.dat' using 1:3 w l t 'vx'"