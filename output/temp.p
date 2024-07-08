print 'vary-dim-e0.050000-d0.000000-q6-s2'
set terminal pdf size 4,3 enhanced font 'Arial,14' butt
set size 1, 1
set output 'vary-dim-e0.050000-d0.000000-q6-s2.pdf'
set key inside top right
set xlabel 'd'
set ylabel 'alpha'
set xtics 1
plot  'vary-dim-e0.050000-d0.000000-q6-s2-1719417661.dat' using 1:4 title 'MinD' with linespoints, 'vary-dim-e0.050000-d0.000000-q6-s2-1719417661.dat' using 1:5 title 'MinR' with linespoints, 'vary-dim-e0.050000-d0.000000-q6-s2-1719417661.dat' using 1:2 title 'Squeeze-u' with linespoints, 'vary-dim-e0.050000-d0.000000-q6-s2-1719417661.dat' using 1:3 title 'UH-Random' with linespoints, vary-dim-e0.050000-d0.000000-q6-s2-1719417661.dat' using 1:6 title 'Breakpoint' with linespoints
