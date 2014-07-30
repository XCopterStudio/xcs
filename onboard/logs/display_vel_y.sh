grep -v register $1 | grep "\bvelocity\b" > vel.dat
grep -v register $1 | grep "\bekf_velocity" > ekf_vel.dat

gnuplot -p -e "plot 'vel.dat' using 1:4 w l t 'vy', 'ekf_vel.dat' using 1:4 w l t 'ekf_vy'"