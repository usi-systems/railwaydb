set term post enh solid "Helvetica" 28

set output "| ps2pdf - time.pdf"

set ylabel "Time in milliseconds"
set xlabel "Number of attributes"

plot \
"x.txt" u 1:2 t "time to partition" w linespoints




