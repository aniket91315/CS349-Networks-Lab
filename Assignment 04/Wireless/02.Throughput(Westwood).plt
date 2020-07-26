set terminal png
set output "02.Throughput(Westwood)"
set title "Throughput Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Throughput"

set xrange [40:1500]
plot "-"  title "Throughput (Kbps)" with linespoints
40 5.22843
44 5.41443
48 5.54403
52 5.70311
60 5.99394
250 13.3692
300 15.2678
552 24.9672
576 25.9965
628 27.8774
1420 58.8303
1500 61.3889
e
