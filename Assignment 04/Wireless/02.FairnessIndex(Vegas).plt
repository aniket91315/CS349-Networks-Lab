set terminal png
set output "02.FairnessIndex(Vegas)"
set title "Fairness Index Vs Packet Size"
set xlabel "Packet Size"
set ylabel "Fairness Index"

set xrange [40:1500]
plot "-"  title "Fairness" with linespoints
40 0.77069
44 0.763673
48 0.754317
52 0.746162
60 0.732234
250 0.596094
300 0.583369
552 0.549309
576 0.548074
628 0.544347
1420 0.52058
1500 0.519583
e
