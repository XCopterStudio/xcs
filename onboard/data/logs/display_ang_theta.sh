grep -v register $1 | grep "[^_]rotation" > theta-ref.dat
grep -v register $1 | grep "[^_]velocity" > vel-ref.dat
grep -v register $1 | grep ekf_rotation > theta.dat
grep -v register $1 | grep ekf_velocity > vel.dat
grep -v register $1 | grep flyControl > fly.dat

gnuplot -p -e "plot 'theta.dat' using 1:4 w l t 'theta', 'theta-ref.dat' using 1:4 w l t 'theta ref', 'vel.dat' using 1:4 w l t 'vy', 'fly.dat' using 1:4 t 'flyControl'"