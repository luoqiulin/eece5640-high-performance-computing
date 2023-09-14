For Question1_1.c

module load openmpi
module load cuda/9.2
mpicc Question1_1.c -o Q1_1
srun --nodes=2 --tasks-per-node=16 Q1_1

----------------------------------------------

For Question1_2.c

module load openmpi
module load cuda/9.2
mpicc Question1_2.c -o Q1_2
srun --nodes=2 --tasks-per-node=16 Q1_2
