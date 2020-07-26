set terminal png
set output "01.Throughput(Vegas)"
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
250 110.248
300 121.564
552 159.658
576 166.765
628 163.617
1420 169.557
1500 169.404
e
