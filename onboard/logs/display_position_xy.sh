grep -v register sample3-out.txt | grep position >pos.dat

gnuplot -p -e "set size square; plot 'pos.dat' using 3:4 w l t 'path'"