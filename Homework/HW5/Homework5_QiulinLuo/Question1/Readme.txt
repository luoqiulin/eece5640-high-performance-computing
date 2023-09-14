For Question1.cu

srun -p reservation --reservation=EECE5640 -N 1 -n 1  --gres=gpu:1 --pty --time=03:00:00 /bin/bash
module load cuda/9.2
nvcc -o Question1 Question1.cu -fopenmp -Xcompiler
./Question1 N
                   ↓
e.g 1024, 32768, 1048576, 33554432