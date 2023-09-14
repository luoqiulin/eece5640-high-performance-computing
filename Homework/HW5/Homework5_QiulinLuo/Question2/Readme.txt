For Question2.cu

srun -p reservation --reservation=EECE5640 -N 1 -n 1  --gres=gpu:1 --pty --time=03:00:00 /bin/bash
module load cuda/9.2
nvcc Question2.cu -o Question2
./Question2