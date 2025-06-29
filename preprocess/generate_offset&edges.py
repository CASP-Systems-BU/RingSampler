# this is used to preprocess synthetic graph
# this file takes the original edges list txt file as input, 
# then generate the binary edges and offset files in the format we want 
import numpy as np
import os

def generate_edges_and_offsets(input_file, total_nodes, output_dir='/mnt/nvme2/yche-bin/small_64'):
    # Ensure the output directory exists
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Read the input file and parse edges
    edges = []

    with open(input_file, 'r') as f:
        for line in f:
            src, dst = map(int, line.split())
            edges.append(dst)

    edges = np.array(edges, dtype=np.uint32)

    # Initialize offsets
    offsets = np.zeros(total_nodes + 1, dtype=np.uint64)

    with open(input_file, 'r') as f:
        current_node = -1
        edge_index = 0
        for line in f:
            src, _ = map(int, line.split())
            if src != current_node:
                if current_node != -1:
                    offsets[current_node + 1] = edge_index
                current_node = src
            edge_index += 1
        offsets[current_node + 1] = edge_index

    for i in range(1, len(offsets)):
        if offsets[i] == 0:
            offsets[i] = offsets[i-1]

    # Save edges and offsets to binary files
    edges_file = os.path.join(output_dir, 'edges.bin')
    offsets_file = os.path.join(output_dir, 'offsets.bin')

    edges.tofile(edges_file)
    offsets.tofile(offsets_file)

    print(f'Edges and offsets files generated in {output_dir}.')
    print(f'Number of edges: {len(edges)}, Number of offsets: {len(offsets)}')

# Example usage
input_file = '/mnt/nvme2/yche-bin/small_64/small_64.txt'  # Replace with the actual path to your file
total_nodes = 134217728  # Replace with the actual number of nodes
generate_edges_and_offsets(input_file, total_nodes)
