set terminal latex
set output 'presentation-gnuplottex-fig2.tex'
unset key
set ylabel "$n$"
set xlabel "$s$"
plot "uniform\_stat\_detail.dat" smooth unique
