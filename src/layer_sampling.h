/*
These are the core functions of io_uring reading, 
including synchronous reading, asynchronous reading and some help functions
*/

#ifndef LAYER_SAMPLING_H
#define LAYER_SAMPLING_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>  
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

struct tuple {
    uint64_t left;
    uint64_t right;
};

// iouring structure to create read requests and store return values
struct io_data {
    uint64_t offset; // store the offset of the value to read
    struct iovec iov; // point tom a set of buffers that have the file data in them
};

// int queue_read(struct io_uring *ring, off_t size, uint64_t offset, int infd, struct io_data *data);
int getRandomInRange(int min, int max);
int get_all_offset(struct io_uring *ring, int numSamples, uint64_t *offset_storage, int *target_to_read, uint64_t *all_offsets_storage, int *neighbor_storage, struct tuple *offset_tuple_storage, int QD, struct io_data *data_storage);
uint64_t mapIndexToOffset(struct tuple *ranges, int numRanges, uint64_t flatIndex);
int queue_read_func(struct io_uring *ring, off_t size, uint64_t offset, int infd);
int get_offsets(int numSamples, uint64_t *offset_storage, int *target_to_read, uint64_t *all_offsets_storage, struct tuple *offset_tuple_storage);
int process_completion_entry(struct io_uring *ring, int *neighbor_storage, int *neighbor_index, int *batch_to_read);
int async_read_offsets_and_store(struct io_uring *ring, uint64_t *index_storage, int offset_num, int QD, int infd, int *neighbor_storage, struct io_data *data_storage, uint64_t *all_offsets_storage);

#endif // LAYER_SAMPLING_H