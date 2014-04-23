set terminal latex
set output 'report-gnuplottex-fig3.tex'
set terminal epslatex color
set ylabel "level"
set xlabel "$d$"
set ytics (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)

levels = 16
xp = 1.3
maxd = 700

set xrange [0:maxd]
set yrange [0:levels + 3]
unset autoscale

start_distance(s) = s**xp
step_distance(s) = s
constrain_lvl(l) = (l > levels - 1) ? (levels - 1) : l

l(d, s) = constrain_lvl( \
(d > start_distance(s)) ? ((d - start_distance(s)) / step_distance(s)) : 0 \
)

plot [d=0:maxd] \
floor(l(d, 20)) with steps lt rgb "black" title "$s = 20$", \
floor(l(d, 10)) with steps lt rgb "blue" title "$s = 10$", \
floor(l(d, 50)) with steps lt rgb "red" title "$s = 50$"
