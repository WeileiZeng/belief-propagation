date
squeue -u $USER --start
#slurm_limits
group_cpus
jobID=976834
jobID=976851
jobID=976860
jobID=976865
jobID=976879
jobID=976895
jobID=976899
scontrol show job $jobID
#sacct -u $USER -l >>temp.log
#sacct -u $USER -S 2018-01-01 -E 2018-08-30 -l | less -S # Type 'q' to quit
