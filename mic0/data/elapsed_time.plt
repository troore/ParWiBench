set terminal postscript enhanced color
set output "rx.eps"

set size 1.2, 1
set grid ytics back
#set key inside left top nobox horizontal

#set boxwidth 2 absolute
set style data histogram
set style histogram cluster gap 1

set style fill solid 1.0 border lt -1

set style line 1 lt 1 lc rgb "#D3D3D3"
set style line 2 lt 1 lc rgb "gray"
set style line 3 lt 1 lc rgb "#A9A9A9"
set style line 4 lt 1 lc rgb "black"

#set xtics rotate

set ylabel "RX-TIME"
#set xlabel "Module"

#plot 'elapsed_time.dat' using 2 t "set-3" ls 2, '' using 3:xtic(1) t "set-4" ls 4
plot 'elapsed_time.dat' using 2 t "Pentium", '' using 3 t "APU", '' using 4:xtic(1) t "Xeon"
#plot 'elapsed_time.dat' using 2:xtic(1) t ""

pause -1 "Hit any key to continue"


