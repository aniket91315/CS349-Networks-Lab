set terminal png
set output "01.Throughput(Veno)"
set title "Throughput Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Throughput"

set xrange [40:1500]
plot "-"  title "Throughput (Kbps)" with linespoints
40 37.5744
44 35.1068
48 37.8756
52 41.0769
60 47.4756
250 120.424
300 143.633
552 222.524
576 232.523
628 252.548
1420 261.239
1500 261.008
e
