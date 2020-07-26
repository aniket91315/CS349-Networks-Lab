set terminal png
set output "02.Throughput(Veno)"
set title "Throughput Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Throughput"

set xrange [40:1500]
plot "-"  title "Throughput (Kbps)" with linespoints
40 5.26219
44 5.43193
48 5.51253
52 5.72109
60 6.0016
250 13.4459
300 15.3099
552 25.0522
576 26.3836
628 28.1443
1420 58.3283
1500 60.8989
e
