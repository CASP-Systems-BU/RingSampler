# this files take the total node_num as input, 
# then generate the train.bin, test.bin, valid.bin randomly for sampling.
# The nodes in the bin files are stored as a list of uint_32

import numpy as np
import os

def generate_split_files(total_nodes, train_percent=0.01, valid_percent=0.005, test_percent=0.002, output_dir='/mnt/nvme2/yche-bin/small_64'):
    # Calculate the number of nodes for each split
    num_train = int(total_nodes * train_percent)
    num_valid = int(total_nodes * valid_percent)
    num_test = int(total_nodes * test_percent)
    
    # Ensure the output directory exists
    print(f'check output directory')
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Generate node IDs and shuffle them
    print(f'Generate node IDs and shuffle them')
    node_ids = np.arange(total_nodes, dtype=np.uint32)
    np.random.shuffle(node_ids)

    # Split the node IDs
    print(f' Split the node IDs')
    train_ids = node_ids[:num_train]
    valid_ids = node_ids[num_train:num_train + num_valid]
    test_ids = node_ids[num_train + num_valid:num_train + num_valid + num_test]

    # Save the splits to binary files
    print(f' Save the splits to binary files')
    train_file = os.path.join(output_dir, 'train.bin')
    valid_file = os.path.join(output_dir, 'valid.bin')
    test_file = os.path.join(output_dir, 'test.bin')

    train_ids.tofile(train_file)
    valid_ids.tofile(valid_file)
    test_ids.tofile(test_file)

    print(f'Train, valid, and test files generated in {output_dir}.')

# Example usage
total_nodes = 134217728  # Replace with the actual number of nodes
generate_split_files(total_nodes)
