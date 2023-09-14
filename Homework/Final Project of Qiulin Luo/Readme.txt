For Workload_1:

Change classification module VGG or GoogleNet in Training.py 

srun -p reservation --reservation=EECE5640 -N 1 -n 1 --gres=gpu:1 --pty --time=03:00:00 /bin/bash 

module load cuda/11.3
module load anaconda3/2022.01
conda create --name pytorch_env python=3.6 -y
source activate pytorch_env
conda install pytorch torchvision=0.2.0 torchaudio numpy cudatoolkit=11.3 -c pytorch -y
python3 Training.py


For Workload_2：

For GPU p100:
srun --partition=gpu --nodes=1 --pty --gres=gpu:p100:1 --ntasks=1 --mem=10GB --time=01:00:00 /bin/bash

For GPU k40m:
srun --partition=gpu --nodes=1 --pty --gres=gpu:k40m:1 --ntasks=1 --mem=10GB --time=01:00:00 /bin/bash

module load cuda/11.3
module load anaconda3/2022.01
conda create --name pytorch_env python=3.6 -y
source activate pytorch_env
conda install pytorch torchvision=0.2.0 torchaudio numpy cudatoolkit=11.3 -c pytorch -y
python3 Training.py


