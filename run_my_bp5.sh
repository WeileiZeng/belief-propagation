make my_bp4.out -f hpcc_makefile

#filename_data="gnuplot/result/my-bp4-iter-9-fb-5-data-100-schedule-3-hpcc.gnudat"
filename_data="gnuplot/result/bp4-iter-9-fb-5-data-1000-sch-3-min-sum.gnudat"
filename_data="gnuplot/result/bp5-test-run.gnudat"

#test run
#./my_bp5.out cores=16 schedule_mode=5 decode_mode_str="min sum" feedback=5 exit_iteration=9 time_out=100.0 num_data_points=1000 filename_data=$filename_data


#longer run
./my_bp5.out cores=16 schedule_mode=5 decode_mode_str="min sum" feedback=5 exit_iteration=9 time_out=100.0 num_data_points=1000 filename_data=$filename_data