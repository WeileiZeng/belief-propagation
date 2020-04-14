#!/bin/bash -l

#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=1G
#SBATCH --time=0-05:00:00     # day-hour:min:sec
#SBATCH --output=stdout/n1t1c32.log
#SBATCH --mail-user=wzeng002@ucr.edu
#SBATCH --mail-type=ALL,TIME_LIMIT_80,TIME_LIMIT_90
#SBATCH --job-name="n1t1c32"
#SBATCH -p intel # This is the default partition, you can use any of the following; intel, batch, highmem, gpu


# Print current date
date

# Load samtools
module load itpp

# Change directory to where you submitted the job from, so that relative paths resolve properly
echo folder: $SLURM_SUBMIT_DIR
cd $SLURM_SUBMIT_DIR


# Print name of node
echo hostname:
hostname

cp my_bp4.out cmd.out
chmod +x cmd.out
echo copied file to cmd.out to prevent conflict on update

filename="bp4-iter-9-fb-5-data-1000-sch-5-min-sum"
filename_data="gnuplot/result/$filename.gnudat"
filename_std="stdout/$filename.log"
echo start job, see out put in $filename_std and $filename_data
./cmd.out cores=32 schedule_mode=5 decode_mode_str="min sum" feedback=5 exit_iteration=9 chunk_time_out=200.0 num_data_points=1000 filename_data=$filename_data >> $filename_std &

filename="bp4-iter-9-fb-5-data-1000-sch-3-min-sum"
filename_data="gnuplot/result/$filename.gnudat"
filename_std="stdout/$filename.log"
echo start job, see out put in $filename_std and $filename_data
./cmd.out cores=32 schedule_mode=3 decode_mode_str="min sum" feedback=5 exit_iteration=9 chunk_time_out=200.0 num_data_points=1000 filename_data=$filename_data >> $filename_std &

wait
echo done sbatch
date
