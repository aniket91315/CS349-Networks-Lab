set terminal png
set output "02.Throughput(Vegas)"
set title "Throughput Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Throughput"

set xrange [40:1500]
plot "-"  title "Throughput (Kbps)" with linespoints
40 5.28384
44 5.34605
48 5.50444
52 5.74087
60 5.97903
250 13.4846
300 15.3105
552 25.3085
576 25.7812
628 27.7281
1420 58.7209
1500 61.4252
e
