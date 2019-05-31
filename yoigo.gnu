set style line 1 \
    linecolor rgb '#0050a0' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 0
set style line 2 \
    linecolor rgb '#b0b0b0' \
    linetype 1 linewidth 1 \
    pointtype 7 pointsize 0
#set title "Consumo de datos" font ",20"
set key right bottom box
a=0
cumulative_sum(x)=(a=a+x,a)
plot 'yoigo.dat' using 1:(cumulative_sum($2)) with linespoints linestyle 1 title 'Consumo total de datos por día', a with linespoints linestyle 2 title 'Consumo actual'