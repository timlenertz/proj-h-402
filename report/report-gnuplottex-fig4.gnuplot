set terminal latex
set output 'report-gnuplottex-fig4.tex'
set terminal epslatex color
set ylabel "$r_{\text{level}}$"
set xlabel "$level$"
set xtics (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)

n = 5000000
nmin = 1000000
rmin = nmin / n
levels = 16

set xrange [0:(levels - 1)]
set yrange [0:1]
unset autoscale

r(l, a) = 1 - (1 - rmin) * (l/(levels - 1))**a
plot [l=0:(levels - 1)] \
r(l, 2) lt rgb "black" title "$a = 2.0$", \
r(l, 1.2) lt rgb "blue" title "$a = 1.2$", \
r(l, 0.4) lt rgb "green" title "$a = 0.4$", \
r(l, 3.8) lt rgb "red" title "$a = 3.8$"
