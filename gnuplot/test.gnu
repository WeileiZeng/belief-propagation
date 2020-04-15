set terminal pdf
set output 'test.pdf'

plot   x , x*x,  'xx.gnudat' using 1:2