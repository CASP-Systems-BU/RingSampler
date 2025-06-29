# gSampler

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
- **gSampler:** Install from [source](https://github.com/gsampler9/gSampler)

## Build Instructions

### Prepare datasets

#### Download Datasets
- [ogbn-papers100M](https://ogb.stanford.edu/docs/nodeprop/#ogbn-papers100M)
- [Friendster](https://snap.stanford.edu/data/com-Friendster.html)

#### Preprocess
gSampler supports OGB datasets, such as ogbn-papers100M and ogbn-products, by loading with DGL dataloader. 

For custimized dataset, checkout `/gnn-sampling/gSampler/preprocess` for preprocessing raw dataset with txt format. 

See gSampler [data preparation instruction](https://github.com/gpzlx1/gsampler-artifact-evaluation/tree/main?tab=readme-ov-file#dataset-preparation) for more details. 

## Run

```
# run gSampler GraphSAGE node-wise sampling
python graphsage_gSampler.py --dataset --batchsize --data-type --use-uva --device --big-batch --samples

# run sample script(sample all support datasets)
bash run_graphsage_gsampler.sh 
```

## Results

### Average sampling time of 1 epoch: batch_size=1024, fanout=[20 15 10]
Sampling mode
- Pure GPU mode: Load graph data to GPU and do sampling with GPU
- GPU w/ UVA: Load graph data to main memory and access data with UVA to GPU to do sampling

### ogbn-papers100m

| Sampling Mode   |Pure GPU| GPU w/ UVA  |
| ------------- |:-------------:| :-----:| 
| avg runtime(s)| 0.897|5.766| 

### Friendster

| Sampling Mode   | Pure GPU       | GPU w/ UVA  |
| ------------- |:-------------:| :-----:| 
| avg runtime(s)| 0.536| 3.281	| 

### Acknowledgement
This directory is a reproduction of gSampler experiments. The source of code is [gSampler](https://github.com/gsampler9/gSampler) and [gsampler-artifact-evaluation](https://github.com/gpzlx1/gsampler-artifact-evaluation). Links to orignal code are attached to the beginning of each file. 


