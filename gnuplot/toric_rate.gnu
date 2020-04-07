#!/usr/bin/gnuplot

# parameter
data_folder='result'

#data_title='data-cycle100'
#data_title='iteration10-cycle1000'
data_title='iteration0-cycle1000'
data_title='my-bp2-iteration0-cycle200'

plot_title='-weight'
plot_title=''

data_file=data_folder.'/'.data_title.'.gnudat'


#plot failure rate versus p
#set terminal pngcairo size 900,600 enhanced font 'Verdana,10'
set terminal pdf

#set output "bp_plot/toric_rate.pdf"
set output "bp_plot/".data_title.plot_title.".pdf"


set key left box
set xlabel "Error probability: p"
set ylabel "Converge rate: P_c"
set logscale y
set format y "10^{%L}"
set logscale x
set format x "10^{%L}"
set xrange [0.01:0.1]
set yrange [0.01:1]
set title "converge rate of BP for toric code ".data_title.plot_title

set linetype cycle 5

#size =5
#size_set ="5 7 9 11 13"

#plot rate
#f(x)=x

size_set="13 11 9 7 5"
color_set="violet blue red yellow black green"

# plot rate
plot for [i=1:5] data_file using (column((i-1)*5+1)):(column((i-1)*5+2)) title 'size '.word(size_set,i) with linespoints ps 0.5 lc i

# plot weight of input error
# plot for [i=1:5] data_file using (column((i-1)*5+1)):(column((i-1)*5+2)) title 'size '.word(size_set,i).' converge' ps 0.5,for [i=1:5] data_file using (column((i-1)*5+1)):(column((i-1)*5+3) + column((i-1)*5+4)+ column((i-1)*5+5)) title 'size '.word(size_set,i).' weight 0+1+2' with lines linecolor i,for [i=1:5] data_file using (column((i-1)*5+1)):(column((i-1)*5+3)) title 'size '.word(size_set,i).' weight 0' linecolor i dashtype ".-" with lines,


#plot data_file using 1:2 title 'size 13' with linespoints,\
#data_file using 6:7 title 'size 11' with linespoints,\
#data_file using 11:12 title 'size 9' with linespoints,\
#data_file using 16:17 title 'size 7' with linespoints,\
#data_file using 21:22 title 'size 5' with linespoints ps 0.5

#plot for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:2 title 'size '.size.': ratio of converge in BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:3 title 'size '.size.': ratio of bad error in rand decoding after BP decoding' with linespoints, for [size in size_set] data_folder.'/rate_versus_p_size_'.size.'.gnudat' using 1:4 title 'size '.size.': ratio of bad error in direct rand decoding' with linespoints,



#pause -1 "hit any key to exit"