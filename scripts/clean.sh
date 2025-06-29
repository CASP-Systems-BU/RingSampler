#!/bin/bash

# can be run by "sudo ../experiments/clean.sh"
# This file is used to clean the cache before running the experiments

sync; echo 1 > /proc/sys/vm/drop_caches
sync; echo 2 > /proc/sys/vm/drop_caches
sync; echo 3 > /proc/sys/vm/drop_caches
