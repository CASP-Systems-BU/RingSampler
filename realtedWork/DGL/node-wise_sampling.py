# Refer to code:
# https://github.com/dmlc/dgl/blob/2.3.x/examples/sampling/node_classification.py

import argparse
import time

import dgl
import dgl.nn as dglnn
import torch
import torch.nn as nn
import torch.nn.functional as F
import torchmetrics.functional as MF
import tqdm
from dgl.data import AsNodePredDataset
from dgl.dataloading import (
    DataLoader,
    MultiLayerFullNeighborSampler,
    NeighborSampler,
)
from ogb.nodeproppred import DglNodePropPredDataset
from data_loader import load_friendster, load_ogbn_papers100M, load_yahoo

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--mode",
        default="mixed",
        choices=["cpu", "mixed", "gpu"],
        help="Training mode. 'cpu' for CPU training, 'mixed' for "
        "CPU-GPU mixed training, 'gpu' for pure-GPU training.",
    )
    parser.add_argument(
        "--dataset",
        default="ogbn-papers100M",
        choices=["ogbn-papers100M", "friendster", "yahoo"],
        help="Choose the dataset: 'ogbn-papers100M' or 'friendster'.",
    )
    args = parser.parse_args()
    if not torch.cuda.is_available():
        args.mode = "cpu"
    print(f"Training in {args.mode} mode.")

    # Load and preprocess dataset based on user choice
    print("Loading data")
    if args.dataset == "ogbn-papers100M":
        g, _, _, _, splitted_idx = load_ogbn_papers100M()
        train_idx = splitted_idx['train']
    elif args.dataset == "friendster":
        g, _, _, _, splitted_idx = load_friendster()
        train_idx = splitted_idx['train']
    elif args.dataset == "yahoo":
        g, _, _, _, splitted_idx = load_yahoo()
        train_idx = splitted_idx['train']
    else:
        raise ValueError("Unsupported dataset")

    # Convert the graph to CSC format
    # g = dataset[0]
    g = g.formats('csc')  
    g = g.to("cuda" if args.mode == "gpu" else "cpu")
    use_uva = args.mode == "mixed"
    device = torch.device("cpu" if args.mode == "cpu" else "cuda")
    
    # Sampling
    print("Sampling...")
    train_idx = train_idx.to(g.device if not use_uva else device)
    sampler = NeighborSampler([20, 15, 10])

    train_dataloader = DataLoader(
        g,
        train_idx,
        sampler,
        device=device,
        batch_size=1024,
        shuffle=True,
        drop_last=False,
        num_workers=0,
        use_uva=use_uva,
    )

    for epoch in range(5):
        t0 = time.time()
        for it, (input_nodes, output_nodes, blocks) in enumerate(train_dataloader):
            pass
        t1 = time.time()
        print(f"{t1 - t0:.4f}")