set terminal postscript eps enhanced color font 'Helvetica,25'
set output 'evolution.eps'

set yrange [400000:600000]

plot "evolution.dat" using 1 title "Dual" with lines, \
     "evolution.dat" using 2 title "Primal" with lines