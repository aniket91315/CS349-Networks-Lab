set terminal png
set output "01.FairnessIndex(Veno)"
set title "Fairness Index Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Fairness Index"

set xrange [40:1500]
plot "-"  title "Fairness" with linespoints
40 0.552956
44 0.55336
48 0.546151
52 0.546382
60 0.550979
250 0.541917
300 0.541397
552 0.541353
576 0.544752
628 0.544803
1420 0.541294
1500 0.541334
e
