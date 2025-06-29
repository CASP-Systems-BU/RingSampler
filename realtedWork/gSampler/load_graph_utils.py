# Code is from
# https://github.com/gpzlx1/gsampler-artifact-evaluation/blob/main/figure7/load_graph_utils.py

from ogb.nodeproppred import DglNodePropPredDataset
import torch
import dgl
import scipy.sparse as sp
import pandas as pd

def load_100Mpapers():
    data = DglNodePropPredDataset(name="ogbn-papers100M",root="/mnt/nvme1n1p1")
    splitted_idx = data.get_idx_split()
    g, labels = data[0]
    g=g.long()
    g.ndata.clear()
    g = dgl.remove_self_loop(g)
    g = dgl.add_self_loop(g)
    return g, None, None, None, splitted_idx

def load_friendster():
    train_id = torch.load("/mnt/nvme2/data/friendster/train.pt")
    splitted_idx = dict()
    splitted_idx['train']=train_id
    bin_path = "/mnt/nvme2/data/friendster/friendster_adj.bin"
    g_list, _ = dgl.load_graphs(bin_path)
    g = g_list[0]
    print("graph loaded")
    g=g.long()
    return g, None,None,None,splitted_idx

def load_livejournal():
    train_id = torch.load("/home/ubuntu/dataset/livejournal/livejournal_trainid.pt")
    splitted_idx = dict()
    splitted_idx['train']=train_id
    coo_matrix = sp.load_npz("/home/ubuntu/dataset/livejournal/livejournal_adj.npz")
    g = dgl.from_scipy(coo_matrix)
    g = dgl.remove_self_loop(g)
    g = dgl.add_self_loop(g)
    g=g.long()
    return g, None, None, None, splitted_idx

def load_synthetic():
    train_id = torch.load("/mnt/nvme2/yche-bin/small_4/train_nodes.pt")
    splitted_idx = dict()
    splitted_idx['train']=train_id
    bin_path = "/mnt/nvme2/yche-bin/small_4/small_4.bin"
    g_list, _ = dgl.load_graphs(bin_path)
    g = g_list[0]
    print("graph loaded")
    g=g.long()
    return g, None,None,None,splitted_idx

def load_yahoo():
    train_id = torch.load("/mnt/nvme1n1p1/yahoo/train_nodes.pt")
    splitted_idx = dict()
    splitted_idx['train']=train_id
    bin_path = "/mnt/nvme1n1p1/yahoo/yahoo_adj.bin"
    g_list, _ = dgl.load_graphs(bin_path)
    g = g_list[0]
    print("graph loaded")
    g=g.long()
    return g, None,None,None,splitted_idx