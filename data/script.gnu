set title "Preys Predators Simulation"
set key
set yrange [0:1]
set xlabel "Time (sec)"
set ylabel "Density"
plot 'data/data1.txt' using 1:2 with lines lw 2 lc 'orange' title 'preys'
replot 'data/data1.txt' using 1:3 with lines lw 2 lc 'blue' title 'predators'
pause 1
reread

