#ifndef IO_URING_UTILS_H
#define IO_URING_UTILS_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

/*
paths of needed files (in casper)
*/ 
#define YAHOO_EDGE_PATH "/mnt/nvme1n1p1/yahoo/edges.bin"
#define YAHOO_OFFSET_PATH "/mnt/nvme1n1p1/yahoo/data/offset.bin"
#define YAHOO_TRAIN_PATH "/mnt/nvme1n1p1/yahoo/data/train_nodes.bin"

#define PAPERS_EDGE_PATH "/mnt/nvme1n1p1/papers100M/edges.bin"
#define PAPERS_OFFSET_PATH "/mnt/nvme1n1p1/papers100M/paper_offset_64.bin"
#define PAPERS_TRAIN_PATH "/mnt/nvme1n1p1/papers100M/train_nodes.bin"

#define FRIENDSTER_EDGE_PATH "/mnt/nvme2/data/friendster/edges.bin"
#define FRIENDSTER_OFFSET_PATH "/mnt/nvme2/data/friendster/offsets.bin"
#define FRIENDSTER_TRAIN_PATH "/mnt/nvme2/data/friendster/train_nodes.bin"

#define SMALL_4_EDGE_PATH "/mnt/nvme2/yche-bin/small_4/edges.bin"
#define SMALL_4_OFFSET_PATH "/mnt/nvme2/yche-bin/small_4/offset_64.bin"
#define SMALL_4_TRAIN_PATH "/mnt/nvme2/yche-bin/small_4/train.bin"

#define SMALL_64_EDGE_PATH "/mnt/nvme2/yche-bin/small_64/edges.bin"
#define SMALL_64_OFFSET_PATH "/mnt/nvme2/yche-bin/small_64/offsets.bin"
#define SMALL_64_TRAIN_PATH "/mnt/nvme2/yche-bin/small_64/train.bin"

static int neighbor_size[3] = {20, 15, 10};

off_t get_file_size(int fd);
void dump_List(const uint32_t *list, int size);
void dump_List_offset(const uint64_t *list, int size);
int setup_context(unsigned entries, struct io_uring *ring);
void shuffle(int *arr, int n);
double calculate_elapsed_time_ms(struct timespec start, struct timespec end);

#endif // IO_URING_UTILS_H
