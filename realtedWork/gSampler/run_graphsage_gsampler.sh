echo "graphsage_gSampler"

# python graphsage_gSampler.py --dataset livejournal --batchsize=512 --big-batch=51200
# python graphsage_gSampler.py --dataset products --batchsize=512 --big-batch=51200
python graphsage_gSampler.py --dataset=papers100m --batchsize=1024 --data-type=long --use-uva --device=cuda --big-batch=1024 --samples="20,15,10"
python graphsage_gSampler.py --dataset=papers100m --batchsize=1024 --data-type=long --use-uva --device=cpu --big-batch=1024 --samples="20,15,10"
python graphsage_gSampler.py --dataset=friendster --batchsize=512 --data-type=long --use-uva --device=cpu --big-batch=1024 --samples="20,15,10"
# python graphsage_gSampler.py --dataset=synthetic --batchsize=1024 --data-type=long --use-uva --device=cuda --big-batch=1024 --samples="20,15,10"
python graphsage_gSampler.py --dataset=friendster --batchsize=512 --data-type=long --use-uva --device=cuda --big-batch=1024 --samples="20,15,10"
