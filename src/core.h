/*
These are the core functions of io_uring reading, 
including synchronous reading, asynchronous reading and some help functions
*/

#ifndef CORE_H
#define CORE_H

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
#include <pthread.h>
#include "utils.h"
#include "random_selection.h"
#include "remove_duplicate.h"

static int infd; // file description of the reading file 
static struct io_uring_sqe *sqe; // iouring submission queue entry
static struct io_uring_cqe *cqe; // iouring completion queue entry
static struct io_uring ring; // iouring ring structure
pthread_barrier_t batch_barrier;
pthread_barrier_t hop_barrier;
pthread_barrier_t dedup_barrier;

// iouring structure to create read requests and store return values
struct io_data {
    uint64_t offset; // store the offset of the value to read
    struct iovec iov; // point tom a set of buffers that have the file data in them
};

struct off_neighbor {
    uint64_t offset;
    uint32_t neighbor; 
};

struct BatchArgs {
    int thread_id;
    int num_threads;
    int QD;
    uint32_t *all_train_nodes;
    int64_t *all_offsets_storage;
    int start_index;
    int num_batches;
    uint32_t *target_storage;
    uint64_t *index_storage;
    uint32_t *neighbor_storage1;
    uint32_t *neighbor_storage2;
    uint32_t *neighbor_storage3;
    struct io_data *data_storage;
    pthread_barrier_t *epoch_barrier;
    pthread_barrier_t *shuffle_barrier;
    int epoch_num;
    char *edge_path; 
    char *train_path;
    int batch_size;
    int training_nodes;
};

int queue_read(struct io_uring *ring, off_t size, uint64_t offset, int infd, struct io_data *data);
int async_read_and_store_with_duplicates(struct io_uring *ring, uint64_t *index_storage, uint32_t *target_storage, int target_num, int QD, int infd, uint32_t *neighbor_storage, int neighbor_index, int num_neighbor, struct io_data *data_storage, uint64_t *all_offsets_storage, unsigned int *seed);
int process_completion(struct io_uring *ring, uint32_t *neighbor_storage, int *neighbor_index, int *batch_to_read);
int get_and_store_offsets(uint32_t target, uint64_t *all_offsets_storage, uint64_t *index_storage, int *total_offsets, int num_neighbor, unsigned int *seed);
void* process_batch_multi_epoch(void *arg);
   
#endif // CORE_H