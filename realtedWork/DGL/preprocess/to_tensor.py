# this file convert the train binary file into the tensor 

import numpy as np
import torch
import argparse
import os

# Set up argument parser to accept the directory path
parser = argparse.ArgumentParser(description='Generate train_nodes.pt from binary file.')
parser.add_argument('--data_dir', type=str, required=True, 
                    help='Directory path for the input and output files (e.g., /mnt/nvme2/data/friendster).')

args = parser.parse_args()

# Define the input and output file paths
data_dir = args.data_dir
input_file = os.path.join(data_dir, 'train_nodes.bin')
output_file = os.path.join(data_dir, 'train_nodes.pt')

# Load the data assuming it is saved as 32-bit integers
train_nodes = np.fromfile(input_file, dtype=np.int32)

# Convert the NumPy array to a PyTorch tensor
train_nodes_tensor = torch.from_numpy(train_nodes)

# Save the tensor to the specified output path
torch.save(train_nodes_tensor, output_file)

# Load the tensor back to verify
loaded_tensor = torch.load(output_file)

# Verify the shape and type to ensure it's as expected
print(f"Shape of loaded tensor: {loaded_tensor.shape}")
print(f"Data type of loaded tensor: {loaded_tensor.dtype}")
