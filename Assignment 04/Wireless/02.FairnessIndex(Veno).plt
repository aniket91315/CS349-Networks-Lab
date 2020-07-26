set terminal png
set output "02.FairnessIndex(Veno)"
set title "Fairness Index Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Fairness Index"

set xrange [40:1500]
plot "-"  title "Fairness" with linespoints
40 0.769631
44 0.760907
48 0.753813
52 0.74477
60 0.731785
250 0.596018
300 0.58336
552 0.549913
576 0.547539
628 0.544237
1420 0.520426
1500 0.519665
e
