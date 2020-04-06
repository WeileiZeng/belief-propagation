#modified from run_bp_save
#change from bp_decoding3 to be_decoding4


#runs bp_decoding3.out and save all the input and output errors

#try bp decoding for toric code with different size, 5,9,25,35
#added size 7,11,13
#p=0.1% to 5%
#100,000 division, 2000 means 2%

#time estimation
#size 9:  65  sec
#size 11: 100 sec
#size 13: 160 sec

clear
make pattern_observer.out

stabilizer_folder=data/toric/stabilizer
error_folder=data/toric/bp_decoding
error_folder=data/toric/temp
#test fold

#size=13
#for size in {5,7,9,11,13}
for size in 7
do
    
    echo start pattern_observer.sh for size ${size}
    date
    #for i in {100..5000..100}
    for i in 1000
	     #for i in {2000..2000..100}
    do
    
	./pattern_observer.out ${stabilizer_folder}/toric_S_x_size_${size}.mm ${stabilizer_folder}/toric_S_z_size_${size}.mm ${error_folder}/toric_S_size_${size}.mm_rate $i 
    done

    wait
    echo finish bp_decoding 10000 cycle for size ${size} x ${size} when p is \in 100,000 division. All error saved
    date
done
    
#echo finish bp_decoding 10000 cycle for size ${size} x ${size} when p is \in 100,000 division. All error saved>> data/toric/bp_converge3/bp_decoding.log &
#date >> data/toric/bp_converge3/bp_decoding.log &
