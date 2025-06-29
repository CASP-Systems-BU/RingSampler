echo "graphsage_dgl"

# Normal sampler 
python node-wise_sampling.py --mode="mixed" --dataset="ogbn-papers100M"
python node-wise_sampling.py --mode="gpu" --dataset="friendster"
