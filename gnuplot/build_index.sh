echo this script will build the index to show all files 

echo "
<head>
  <title>BP gnuplot</title>
</head>
" >index.html

echo "<h2>result in hpcc. Latest first</h2>" >> index.html


for f in $(ls -t hpcc_bp_plot/*.pdf)
do
echo $f

echo "<embed src=\"https://drive.google.com/viewerng/
viewer?embedded=true&url=https://cluster.hpcc.ucr.edu/~wzeng002/gnuplot_bp/$f\" 
 width=\"500\" height = \"320\" >" >> index.html

done
 
