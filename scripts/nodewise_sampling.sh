#!/bin/bash

# can be run by "sudo ../experiments/nodewise_sampling.sh"
nodewise_sampler="/home/cc/RingSampler/build/test_multi_epoch"

# nodewise_sampler takes inputs: <dataset> <QD> <num_threads> <epoch_num> <batch_size>
# dataset: 
    # help: what dataset to load for sampling
    # choies: yahoo, papers, friendster
# QD:
    # help: size of io_uring SQ and CQ ring buffer.
    # value: 2^n, n>=2
# num_thread: 
    # help: how many threads to employ
    # value: in the range of [0, max threads the machine has] 
# epoch_num:
    # help: how many epoches to sample
    # value: integer >= 1
# batch_size:
    # help: size of each mini-batch. how many nodes to be sampled as a group
    # value: [1, target_node_num]

# run yahoo
$nodewise_sampler yahoo 512 64 5 1024

# run ogbn-papers100M
$nodewise_sampler papers 512 64 5 1024

# run friendster
$nodewise_sampler friendster 512 64 5 1024

# run liveJournal
$nodewise_sampler liveJournal 512 64 5 1024