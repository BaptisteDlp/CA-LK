set title "Preys Predators Simulation"
unset key


set xlabel "Preys"
set ylabel "Predators"

plot 'data/data2.txt' using 2:3 with lines lw 2 lc 'red'
 pause 1
 reread
