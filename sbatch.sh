#!/bin/bash -l

#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=1G
#SBATCH --time=0-00:30:00     # 1 day and 15 minutes
#SBATCH --output=n1t2c32.log
#SBATCH --mail-user=wzeng002@ucr.edu
#SBATCH --mail-type=ALL,TIME_LIMIT_80,TIME_LIMIT_90
#SBATCH --job-name="n1t2c32"
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

# . run_random_concatenation.sh $1
./my_bp4.out &
./my_bp4.out &
wait

echo done sbatch
date