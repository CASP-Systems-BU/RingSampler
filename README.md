# RingSampler

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

## Build Instructions

### Get and preprocess datasets
- [ogbn-papers100M](https://ogb.stanford.edu/docs/nodeprop/#ogbn-papers100M)
- [yahoo](https://webscope.sandbox.yahoo.com/catalog.php?datatype=&guccounter=1)
- [Friendster](https://snap.stanford.edu/data/com-Friendster.html)

### Build

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```

## Run

```
# run node-wise sampling for multi-epoch
./test_multi_epoch <dataset> <QD> <num_threads> <epoch_num> <batch_size>

# run sample script(sample all support datasets)
../experiments/nodewise_sampling.sh
```

## Results

### Average sampling time of 1 epoch: QD=512, batch_size=1024, fanout=[20 15 10]

### Yahoo

| Thread Number    | 1        | 4  | 8 | 16 | 32 | 64 |
| ------------- |:-------------:| :-----:| :-----:| :-----:| :-----:| :-----:| 
| avg runtime(s)| 276.578	| 73.283	| 37.831	| 21.029	| 10.412	| 6.26|

### ogbn-papers100m

| Thread Number    | 1        | 4  | 8 | 16 | 32 | 64 |
| ------------- |:-------------:| :-----:| :-----:| :-----:| :-----:| :-----:| 
| avg runtime(s)| 118.011	| 30.571	| 15.793	| 8.863	| 4.381	| 2.609|

### Friendster

| Thread Number    | 1        | 4  | 8 | 16 | 32 | 64 |
| ------------- |:-------------:| :-----:| :-----:| :-----:| :-----:| :-----:| 
| avg runtime(s)| 250.499	| 65.035	|33.251	| 18.983	|9.276	|5.415 |

### Preview of io_uring sampling 

Here is the diagram that visualize the implementation:
<img width="881" alt="Screenshot 2024-09-22 at 2 48 13 AM" src="https://github.com/user-attachments/assets/aed8117e-ce0e-4c8f-b686-c31700a1e024">

