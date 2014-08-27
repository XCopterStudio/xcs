grep -v register $1 | grep "[^_]rotation" >rot.dat
grep -v register $1 | grep "ekf_rotation" >rot-ekf.dat
grep -v register $1 | grep "ekf_velocityPsi" > vel.dat
grep -v register $1 | grep "velocity" > vel_ref.dat
grep -v register $1 | grep "flyControl" > fly.dat

gnuplot -p -e "plot 'rot.dat' using 1:5 w l t 'psi', 'rot-ekf.dat' using 1:5 w l t 'psi-ekf', 'vel.dat' using 1:3 w l t 'velocity-psi', 'vel_temp.dat' using 1:5 w l t 'velocity-psi_ref', 'fly.dat' using 1:5 t 'fly'"