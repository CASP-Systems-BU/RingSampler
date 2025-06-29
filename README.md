# RingSampler: GNN sampling on large-scale graphs with io_uring

This repository includes the implementation of RingSampler described in the [HotStorage '25 paper](https://dl.acm.org/doi/10.1145/3736548.3737829).

You can cite the paper using the BibTeX below:

```
@inproceedings{10.1145/3736548.3737829,
author = {Chen, Qixuan and Song, Yuhang and Martinez, Melissa and Kalavri, Vasiliki},
title = {RingSampler: GNN sampling on large-scale graphs with io_uring},
year = {2025},
isbn = {9798400719479},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
url = {https://doi.org/10.1145/3736548.3737829},
doi = {10.1145/3736548.3737829},
abstract = {Neighborhood sampling is a critical computation step in graph learning with Graph Neural Networks (GNNs), often accounting for the majority of the training time. To mitigate this bottleneck and scale training to very large graphs, existing approaches offload the sampling computation to GPUs or computational storage, such as SmartSSDs. Given the ubiquity of multi-core CPUs and high-throughput SSDs, we investigate a simpler design that performs CPU-based sampling, making GPU resources fully available to the aggregation stage of training instead. We propose RingSampler, a new GNN sampling system that leverages io_uring to support efficient training of billion-edge graphs on a single machine. RingSampler parallelizes sampling by transparently assigning mini-batches to threads and effectively overlapping computation with I/O operations. Our results show that RingSampler significantly outperforms SmartSSD-based sampling on large graphs and is competitive with GPU-accelerated approaches on graphs that fit in main memory.},
booktitle = {Proceedings of the 17th ACM Workshop on Hot Topics in Storage and File Systems},
pages = {52–60},
numpages = {9},
keywords = {Graph Neural Networks, Neighborhood sampling, io_uring},
location = {Boston, MA, USA},
series = {HotStorage '25}
}
```

## Configuration

#### Hardware
- **CPU:** MD EPYC 7713P 64C/128T

- **DRAM size:** 252GB

- **Disk:** 4TB Samsung SSD

- **GPU:**  NVIDIA A100 80GB GPU (not used in io_uring system)


#### Software
- **OS:** Ubuntu20.04
- **CMake:** 3.20.2
- **liburing:** build from [source](https://github.com/axboe/liburing)

## Build and Running Instructions

### Repository organization
- `src/`: Contains the core implementation of RingSampler, including the use of io_uring, asynchronous sampling logic, and supporting utility functions.
- `preprocess/`: Includes scripts to generate the required binary files (edges.bin, offset.bin, train_nodes.bin) for each dataset from raw text-based input.
- `tests/`: Contains code to run RingSampler, performing multi-epoch GraphSAGE sampling on various datasets.
- `scripts/`: Provides example scripts to run sampling on different datasets with configurable parameters.


### Download datasets
- [ogbn-papers100M](https://ogb.stanford.edu/docs/nodeprop/#ogbn-papers100M)
- [yahoo](https://webscope.sandbox.yahoo.com/catalog.php?datatype=&guccounter=1)
- [Friendster](https://snap.stanford.edu/data/com-Friendster.html)
- [Synthetic datasets](https://github.com/RapidsAtHKUST/Graph500KroneckerGraphGenerator)

### Datasets Preprocess
To perform sampling with RingSampler, each dataset must be preprocessed into the following binary files:

- `edges.bin`: A list of destination node IDs (`uint32_t`)
- `offset.bin`: Offset indices for each node’s neighbor list (`uint64_t`)
- `train_nodes.bin`: A list of training node IDs (`uint32_t`)

For a detailed explanation of these files and their structure, please refer to **Section 3.1 (System Overview)** of the paper.

Preprocessing code is provided in the /gnn-sampling/preprocess/, which converts raw .txt edge list formats into the required binary files.

### Build

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```

## Run

Before running, make sure to:
- Update dataset paths in: `src/utils.h`
- Set the correct training node count in: `tests/test_multi_epoch.c` (variable: training_nodes)

```
# run node-wise sampling for multi-epoch
./test_multi_epoch <dataset> <QD> <num_threads> <epoch_num> <batch_size>

# run sample script(sample all support datasets)
../experiments/nodewise_sampling.sh
```

### Preview of RingSampler  

Here is the diagram that visualize the implementation:
<img width="881" alt="Screenshot 2024-09-22 at 2 48 13 AM" src="https://github.com/user-attachments/assets/aed8117e-ce0e-4c8f-b686-c31700a1e024">

