# DGL

## Configuration

#### Hardware
- **CPU:** MD EPYC 7713P 64C/128T

- **DRAM size:** 252GB

- **Disk:** 4TB Samsung SSD

- **GPU:**  NVIDIA A100 80GB GPU (not used in io_uring system)


#### Software
- **OS:** Ubuntu20.04
- **g++/gcc:** 9.4.0
- **CMake:** 3.20.2
- **Python:** 3.8.10
- **Pytorch:** 2.3.1+cu121
- **DGL:** 2.3.0+cu121

## Build Instructions

### Prepare datasets

#### Download Datasets
- [ogbn-papers100M](https://ogb.stanford.edu/docs/nodeprop/#ogbn-papers100M)
- [Friendster](https://snap.stanford.edu/data/com-Friendster.html)

#### Preprocess
DGL supports OGB datasets, such as ogbn-papers100M and ogbn-products, by loading with DGL dataloader. 

For custimized dataset, checkout `/gnn-sampling/DGL/preprocess` for preprocessing raw dataset with txt format and converting train.bin file to tensor format.

See this DGL [guide](https://docs.dgl.ai/api/python/dgl.graphbolt.html#dataset) for more details about dataset preparation.

## Run

```
# make sure you correct the path to the datasets of the dataloders (`data_loader.py`) before you run

# run DGL graphBolt Sampler
python sampling-graphBolt.py --dataset --mode

# run DGL Normal Sampler
python sampling.py --mode
```

### Acknowledge
The code is referring to the examples from [DGL branch 2.3.x](https://github.com/dmlc/dgl/tree/2.3.x). Links to orignal code are attached to the beginning of each file. 


