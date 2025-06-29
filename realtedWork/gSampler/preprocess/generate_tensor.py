# gerenate input files for gSampler
import numpy as np

# Replace 'fname' with your binary file's path
fname = '/mnt/nvme2/data/friendster/train_nodes.bin'

# Load the data assuming it is saved as 32-bit integers
train_nodes = np.fromfile(fname, dtype=np.int32)
import torch

# Convert the NumPy array to a PyTorch tensor
train_nodes_tensor = torch.from_numpy(train_nodes)
# Replace 'output_path' with the desired .pt file output path
output_path = '/mnt/nvme2/data/friendster/train_nodes.pt'

# Save the tensor
torch.save(train_nodes_tensor, output_path)
# Load the tensor back to verify
loaded_tensor = torch.load(output_path)

# Verify the shape and type to ensure it's as expected
print(loaded_tensor.shape)
print(loaded_tensor.dtype)
