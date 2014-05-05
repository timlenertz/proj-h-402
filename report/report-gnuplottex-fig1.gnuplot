set terminal latex
set output 'report-gnuplottex-fig1.tex'
set terminal epslatex color
set xlabel "$l$"
set ylabel "$n / |P|$"
plot \
"stat\_dragon.dat" using 1:($2/3609599+0*$2) smooth unique linetype rgb "black" title "dragon.ply", \
"stat\_statuette.dat" using 1:($2/4999995+0*$2) smooth unique linetype rgb "red" title "statuette.ply", \
"stat\_scan.dat" using 1:($2/681124+0*$2) smooth unique linetype rgb "blue" title "scan.ply"
