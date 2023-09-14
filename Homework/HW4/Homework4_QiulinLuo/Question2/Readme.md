for the file Question2_1.cpp

module load gcc
module load openmpi
module purge
mpicc -lstdc++ Question2_1.cpp -o Q2_1
mpirun -np 1 ./Q2_1

for the file Question2_2.cpp

module load gcc
module load openmpi
module purge
mpicc -lstdc++ Question2_2.cpp -o Q2_2
mpirun -np 1 ./Q2_2

Use sample.script to change nodes.
