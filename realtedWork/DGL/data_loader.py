# Refer to code:
# https://github.com/gpzlx1/gsampler-artifact-evaluation/blob/main/figure7/load_graph_utils.py


import argparse
import time

import dgl
import torch
import torch.nn as nn
import torch.nn.functional as F
import tqdm
from dgl.data import AsNodePredDataset
from dgl.dataloading import DataLoader, NeighborSampler
from ogb.nodeproppred import DglNodePropPredDataset

# Load Friendster dataset
def load_friendster():
    train_id = torch.load("/mnt/nvme2/data/friendster/train.pt")
    train_id = train_id.long()  
    splitted_idx = dict()
    splitted_idx['train']=train_id
    bin_path = "/mnt/nvme2/data/friendster/friendster_adj.bin"
    g_list, _ = dgl.load_graphs(bin_path)
    g = g_list[0]
    print("graph loaded")
    g=g.long()
    return g, None,None,None,splitted_idx

# Load OGBN-Papers100M dataset
def load_ogbn_papers100M():
    data = DglNodePropPredDataset(name="ogbn-papers100M",root="/mnt/nvme1n1p1")
    splitted_idx = data.get_idx_split()
    g, labels = data[0]
    g=g.long()
    g.ndata.clear()
    g = dgl.remove_self_loop(g)
    g = dgl.add_self_loop(g)
    return g, None, None, None, splitted_idx

def load_yahoo():
    train_id = torch.load("/mnt/nvme1n1p1/yahoo/train_nodes.pt")
    train_id = train_id.long()  
    splitted_idx = dict()
    splitted_idx['train']=train_id
    bin_path = "/mnt/nvme1n1p1/yahoo/yahoo_adj.bin"
    g_list, _ = dgl.load_graphs(bin_path)
    g = g_list[0]
    print("graph loaded")
    g=g.long()
    return g, None,None,None,splitted_idx