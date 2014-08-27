grep -v register $1 | grep "ekf_position" >pos.dat
grep -v register $1 | grep "ekf_velocity\b" >vel.dat
grep -v register $1 | grep "altitude\b" >alt-ref.dat

gnuplot -p -e "set yrange [-2:2]; plot 'pos.dat' using 1:5 w l t 'ekf-z', 'alt-ref.dat' u 1:3 w l t 'z ref', 'vel.dat' u 1:5 w l t 'velocity z'"