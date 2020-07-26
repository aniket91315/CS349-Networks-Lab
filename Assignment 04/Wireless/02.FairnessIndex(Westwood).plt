set terminal png
set output "02.FairnessIndex(Westwood)"
set title "Fairness Index Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Fairness Index"

set xrange [40:1500]
plot "-"  title "Fairness" with linespoints
40 0.768227
44 0.760151
48 0.752712
52 0.745738
60 0.730755
250 0.595852
300 0.583616
552 0.549389
576 0.547657
628 0.544104
1420 0.520479
1500 0.519423
e
