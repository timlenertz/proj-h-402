set terminal latex
set output 'presentation-gnuplottex-fig5.tex'
set terminal epslatex color
set ylabel "$r$"
set xlabel "$d$"

levels = 16
xp = 1.3
maxd = 800
n = 5000000
nmin = 1000000
rmin = nmin / n
a = 2
b(s) = 250 / s

start_distance(s) = b(s)**xp
step_distance(s) = b(s)
constrain_lvl(l) = (l > levels - 1) ? (levels - 1) : l

r(l) = 1 - (1 - rmin) * (l/(levels - 1))**a

l(d, s) = constrain_lvl( \
(d > start_distance(s)) ? ((d - start_distance(s)) / step_distance(s)) : 0 \
)

rd(d, s) = r( 0.0 + floor( l(d, s) ) )
rdc(d, s) = r( 0.0 + l(d, s) )

plot [d=0:maxd] \
rd(d, 20) with steps lt rgb "black" title "$s = 20$", \
rd(d, 10) with steps lt rgb "blue" title "$s = 10$", \
rd(d, 50) with steps lt rgb "red" title "$s = 50$"
