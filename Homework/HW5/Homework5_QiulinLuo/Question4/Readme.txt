For Question4.cu （change threads, blocks or iterations in the code)

srun -p reservation --reservation=EECE5640 -N 1 -n 1  --gres=gpu:1 --pty --time=03:00:00 /bin/bash
module load cuda/9.2
nvcc Question4.cu -o Question4
./Question4