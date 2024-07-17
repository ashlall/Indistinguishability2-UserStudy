print 'vary-epsilon-2-63383-d0.000000-q6-s2'
set terminal pdf size 4,3 enhanced font 'Arial,14' butt
set size 1, 1
set output 'vary-epsilon-2-63383-d0.000000-q6-s2.pdf'
set key inside top right
set xlabel 'epsilon'
set ylabel 'alpha'
set logscale x
set xrange [0.001:0.1]
plot  'vary-epsilon-2-63383-d0.000000-q6-s2-1720468161.dat' using 1:4 title 'MinD' with linespoints, 'vary-epsilon-2-63383-d0.000000-q6-s2-1720468161.dat' using 1:5 title 'MinR' with linespoints, 'vary-epsilon-2-63383-d0.000000-q6-s2-1720468161.dat' using 1:2 title 'Squeeze-u' with linespoints, 'vary-epsilon-2-63383-d0.000000-q6-s2-1720468161.dat' using 1:3 title 'UH-Random' with linespoints, 'vary-epsilon-2-63383-d0.000000-q6-s2-1720468161.dat' using 1:6 title 'Breakpoint' with linespoints
