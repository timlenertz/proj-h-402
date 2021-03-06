set terminal latex
set output 'presentation-gnuplottex-fig6.tex'
set terminal epslatex color
set xlabel "$s$"
set ylabel "$n / n_{\min}$"
set yrange [0.0:1.1]

nmin = 1000000
plot \
"setting\_output\_stat/statuette\_inside\_cm8.dat" using 1:($2/nmin+0*$2) smooth unique title "statuette inside cm8", \
"setting\_output\_stat/statuette\_far\_cm8.dat" using 1:($2/nmin+0*$2) smooth unique title "statuette far cm8", \
"setting\_output\_stat/dragon\_inside\_cm8.dat" using 1:($2/nmin+0*$2) smooth unique title "dragon inside cm8", \
"setting\_output\_stat/dragon\_inside\_c.dat" using 1:($2/nmin+0*$2) smooth unique title "dragon inside c", \
"setting\_output\_stat/dragon\_closeup\_cm8.dat" using 1:($2/nmin+0*$2) smooth unique title "dragon closeup cm8"
