print 'vary-n-e0.050000-d0.000000-q9-s2'
set terminal pdf size 4,3 enhanced font 'Arial,14' butt
set size 1, 1
set output 'vary-n-e0.050000-d0.000000-q9-s2.pdf'
set key inside top right
set logscale x
set xlabel 'n'
set ylabel 'alpha'
plot  'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:2 title 'Squeeze-U' with linespoints, 'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:3 title 'TT-Breakpoint' with linespoints, 'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:4 title 'UH_Random' with linespoints, 'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:5 title 'MinD' with linespoints, 'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:6 title 'MinR' with linespoints, 'vary-n-e0.050000-d0.000000-q9-s2-1721270816.dat' using 1:7 title 'Breakpoint' with linespoints
