set terminal latex
set output 'report-gnuplottex-fig1.tex'
unset key
set ylabel "n"
set xlabel "s"
plot "uniform\_stat.dat" smooth unique
