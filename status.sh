date
squeue -u $USER --start
#slurm_limits
group_cpus

jobID=978199
scontrol show job $jobID
#sacct -u $USER -l >>temp.log
#sacct -u $USER -S 2020-04-11 -l | less -S # Type 'q' to quit

# sacct -u $USER -S 2020-01-01 -E 2020-08-30 -l | less -S # Type 'q' to quit
