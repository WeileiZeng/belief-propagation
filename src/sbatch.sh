#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=1G
#SBATCH --time=0-00:10:00     # day-hour:min:sec
#SBATCH --output=n1t1cpu32-s32.log
#SBATCH --mail-user=wzeng002@ucr.edu
#SBATCH --mail-type=ALL,TIME_LIMIT_80,TIME_LIMIT_90
#SBATCH --job-name="openmp_test"
#SBATCH -p intel # This is the default partition, you can use any of the following; intel, batch, highmem, gpu


# Print current date
# date

# Load samtools
#module load itpp

# Change directory to where you submitted the job from, so that relative paths resolve properly
echo folder: $SLURM_SUBMIT_DIR
cd $SLURM_SUBMIT_DIR


# Print name of node
echo hostname:
hostname

echo time:
date
./openmp_test.out 
#>> std.log
date

wait
echo done sbatch
#date
