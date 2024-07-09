print 'fix-totalshown-3-100000-e0.050000-d0.000000-n24'
set terminal pdf size 4,3 enhanced font 'Arial,14' butt
set size 1, 1
set output 'fix-totalshown-3-100000-e0.050000-d0.000000-n24.pdf'
set key inside top right
set xlabel 's'
set ylabel 'alpha'
set xtics 1
plot  'fix-totalshown-3-100000-e0.050000-d0.000000-n24-1720475126.dat' using 1:4 title 'MinD' with linespoints, 'fix-totalshown-3-100000-e0.050000-d0.000000-n24-1720475126.dat' using 1:5 title 'MinR' with linespoints, 'fix-totalshown-3-100000-e0.050000-d0.000000-n24-1720475126.dat' using 1:2 title 'Squeeze-u' with linespoints, 'fix-totalshown-3-100000-e0.050000-d0.000000-n24-1720475126.dat' using 1:3 title 'UH-Random' with linespoints, 'fix-totalshown-3-100000-e0.050000-d0.000000-n24-1720475126.dat' using 1:6 title 'Breakpoint' with linespoints
