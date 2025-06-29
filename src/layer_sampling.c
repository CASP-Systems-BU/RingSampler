// here are the core functions of layer sampling

#include "layer_sampling.h"

int queue_read_func(struct io_uring *ring, off_t size, uint64_t offset, int infd) {

    struct io_uring_sqe *sqe;
    struct io_data *data;

    data = malloc(size + sizeof(*data));
    if (!data)
        return 1;

    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        return 2;
    }

    data->offset = offset; // offset of the value to read
    data->iov.iov_base = malloc(size);  // Starting address of the structure
    if (!data->iov.iov_base) {
        free(data);
        return 3;
    }
    data->iov.iov_len = size; // size of data to read

    io_uring_prep_readv(sqe, infd, &data->iov, 1, offset); // Setup a readv operatio
    io_uring_sqe_set_data(sqe, data); // Set user data
    return 0;
}

// Function to generate a random number in a given range
int getRandomInRange(int min, int max) {
    return rand() % (max - min) + min;
}

uint64_t mapIndexToOffset(struct tuple *ranges, int numRanges, uint64_t flatIndex) {
    uint64_t cumulative = 0;

    for (int i = 0; i < numRanges; i++) {
        uint64_t start = ranges[i].left;
        uint64_t end = ranges[i].right;
        uint64_t rangeSize = end - start;

        if (flatIndex < cumulative + rangeSize) {
            // Calculate the relative position within the range
            return start + (flatIndex - cumulative);
        }
        cumulative += rangeSize;
    }
    
    return (uint64_t)-1; // Return -1 if the index is out of bounds
}

int get_offsets(int numSamples, uint64_t *offset_storage, int *target_to_read, uint64_t *all_offsets_storage, struct tuple *offset_tuple_storage) {
    int total_neighbors = 0; // total number of neighbors we sampled
    int total_offsets = 0; // total number of offsets all targets have 
    int submitted_offsets = 0; // number of offset we've processed
    struct io_data *data;
    int neighbor_index = 0;
    int ret; 

    // for each target node, get their offsets
    for (int target_num = 0; target_num < numSamples; target_num++) {
        int target = target_to_read[target_num];

        // get the neighborhood offset of the target node 
        offset_tuple_storage[target_num].left = all_offsets_storage[target];
        offset_tuple_storage[target_num].right = all_offsets_storage[target+1];
        total_offsets += offset_tuple_storage[target_num].right - offset_tuple_storage[target_num].left;
    }
    
    // get numSamples random offsets form the ranges and store them in the offset_storage
    for (int i = 0; i < numSamples; i++) {
        int randomIndex = getRandomInRange(0, total_offsets);
        offset_storage[i] = mapIndexToOffset(offset_tuple_storage, numSamples, randomIndex);
    }

    // dump_List_offset(offset_storage, numSamples);

    return numSamples;
}   

int async_read_offsets_and_store(struct io_uring *ring, uint64_t *index_storage, int offset_num, int QD, int infd, int *neighbor_storage, struct io_data *data_storage, uint64_t *all_offsets_storage) {

    int sqe_num = 0; // keep track of the read rerquest number we submitted for the SQ
    int completion_to_wait = 0; // keep track the number of read requests within a batch 
    int submitted_offsets = 0; // keep track of the number of offsets we submtted in this batch 
    struct io_data *data;
    int neighbor_index = 0;
    uint64_t offset;
    int ret; 

    // submit the offsets of the first ring
    while (sqe_num < QD && submitted_offsets < offset_num) {
        offset = index_storage[submitted_offsets] * sizeof(int);
        if (queue_read_func(ring, sizeof(int), offset, infd)) {
            printf("Error creating SQE for offset %jd\n", offset);
            return 1;
        }
        sqe_num++;
        submitted_offsets++;
    }
    // submit the first SQ ring
    ret = io_uring_submit(ring);
    if (ret < 0) {
        fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
        return 1;
    }
    completion_to_wait = sqe_num;
    sqe_num = 0;

    while (submitted_offsets < offset_num || completion_to_wait > 0) {

        // before we wait for the completion, 
        // we create prepare the SQ ring for the next group of offsets if we have any unprocessed offsets 
        while (sqe_num < QD && submitted_offsets < offset_num) {
            offset = index_storage[submitted_offsets] * sizeof(int);
            if (queue_read_func(ring, sizeof(int), offset, infd)) {
                printf("Error creating SQE for offset %jd\n", offset);
                return 1;
            }
            sqe_num++;
            submitted_offsets++;
        }

        // time how long it takes to get a batch of completion (get a CQ)
        while (completion_to_wait > 0) {
            // check if there is any completion entry
            if (process_completion_entry(ring, neighbor_storage, &neighbor_index, &completion_to_wait)) {
                return 1; // Handle error
            }
        }

         // submit the SQ we prepared
        if (sqe_num != 0) {
            ret = io_uring_submit(ring);
            if (ret < 0) {
                fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
                return 1;
            }
            completion_to_wait = sqe_num;
            sqe_num = 0;
        }
    }
    return submitted_offsets;
}

int process_completion_entry(struct io_uring *ring, int *neighbor_storage, int *neighbor_index, int *batch_to_read) {
    int ret;
    struct io_uring_cqe *cqe;
    struct io_data *data;

    ret = io_uring_wait_cqe(ring, &cqe);
    if (ret < 0) {
        fprintf(stderr, "io_uring_wait_cqe: %s\n", strerror(-ret));
        return 1;
    }

    data = io_uring_cqe_get_data(cqe);
    if (cqe->res < 0) {
        fprintf(stderr, "cqe failed: %s\n", strerror(-cqe->res)); 
        return 1;
    }

    int *int_array = (int *)data->iov.iov_base;
    neighbor_storage[*neighbor_index] = int_array[0];
    (*neighbor_index)++;
    uint64_t current_offset = data->offset;

    io_uring_cqe_seen(ring, cqe);
    (*batch_to_read)--;

    return 0;
}