#plot failure rate versus p
set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
#set output "iteration/rate-bp4-iteration5-cycle3000.png"
#set output "iteration/rate-bp4-iteration0-cycle3000.png"
set output "iteration/rate-bp4-iteration20-compare0-cycle3000.png"

set key left box
set xlabel "Error probability: p"
set ylabel "failure rate: P_{nc} and P_{bad}"
set logscale y
set format y "10^{%L}"
set logscale x
set format x "10^{%L}"
#set xrange [0.015:0.035]
#set yrange [0:0.04]
#plot both BP and rand for size 5,7,9,11,13

#size =5
size_set ="5 7 9 11 13"
#data_folder='../data/toric/bp_converge3/gnuplot/cycle10000/'
#data_folder='../data/toric/bp_decoding/gnuplot'
#data_folder='../data/toric/bp_decoding3/gnuplot'
data_folder='../data/toric/bp_decoding4/gnuplot'
data_folder2='../data/toric/bp_decoding4-iteration0-cycle3000/gnuplot'

#plot rate
#f(x)=x

#plot for BP decoding
#plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'size '.size.': ratio of nonconvergence after BP decoding' with linespoints ps 0.5, 10*x, 100*x*x

#comparison
plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'size '.size.': ratio of nonconvergence after BP decoding' with lines,\
for [size in size_set] data_folder2.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'iter0-size '.size.': ratio of nonconvergence after BP decoding' with points ps 0.5, 10*x, 100*x*x

#replot x,x*x

#plot for rand decoding after BP
#plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:3 title 'size '.size.': ratio of bad error after rand decoding' with linespoints,

#plot rand after BP and rand directly
#plot  for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:3 title 'size '.size.': ratio of bad error in rand decoding after BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:4 title 'size '.size.': ratio of bad error in direct rand decoding' with linespoints,


#plot for both BP and rand after BP
#plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'size '.size.': ratio of converge after BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:3 title 'size '.size.': ratio of bad error after rand decoding' with linespoints,

#plot for three
#plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'size '.size.': ratio of converge in BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:3 title 'size '.size.': ratio of bad error in rand decoding after BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:4 title 'size '.size.': ratio of bad error in direct rand decoding' with linespoints,

#pause -1 "hit any key to exit"