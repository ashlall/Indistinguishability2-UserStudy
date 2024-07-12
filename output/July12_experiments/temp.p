print 'vary-q-4-21961-e0.050000-d0.000000-s2'
set terminal pdf size 4,3 enhanced font 'Arial,14' butt
set size 1, 1
set output 'vary-q-4-21961-e0.050000-d0.000000-s2.pdf'
set key inside top right
set xlabel 'q'
set ylabel 'alpha'
plot  'vary-q-4-21961-e0.050000-d0.000000-s2-1720794161.dat' using 1:4 title 'MinD' with linespoints, 'vary-q-4-21961-e0.050000-d0.000000-s2-1720794161.dat' using 1:5 title 'MinR' with linespoints, 'vary-q-4-21961-e0.050000-d0.000000-s2-1720794161.dat' using 1:3 title 'UH-Random' with linespoints, 'vary-q-4-21961-e0.050000-d0.000000-s2-1720794161.dat' using 1:6 title 'Breakpoint' with linespoints
