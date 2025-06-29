/*
    These are the core functions of io_uring reading, 
    including synchronous reading, asynchronous reading and some help functions
*/

#include "core.h"

/*
    ring - iouring ring structure
    size - the size of the value we want to read 
    offset - the offset of the value we want to read
    infd - file descripton
    data - the pre-allocated data struct that we send with request
    take an offset and create an submission queue entry/read request of this offset
*/
int queue_read(struct io_uring *ring, off_t size, uint64_t offset, int infd, struct io_data *data) {

    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        return 1;
    }

    data->offset = offset; // offset of the value to read
    data->iov.iov_len = size; // size of data to read

    io_uring_prep_readv(sqe, infd, &data->iov, 1, offset); // Setup a readv operatio
    io_uring_sqe_set_data(sqe, data); // Set user data for the entry
    return 0;   
}

/*
    This function will find the neighobrhoods of a target, 
    then store the offsets of the num_neighbor randomly selected neighbors to storage
*/
int get_and_store_offsets(uint32_t target, uint64_t *all_offsets_storage, uint64_t *index_storage, int *total_offsets, int num_neighbor, unsigned int *seed) {
    // get the neighborhood range
    uint64_t left = all_offsets_storage[target];
    uint64_t right = all_offsets_storage[target + 1];

    // store the randomly selected neighbors' offsets to storage
    if ((right - left) <= num_neighbor) {
        for (uint64_t i = left; i < right; i++) {
            index_storage[*total_offsets] = i;
            (*total_offsets)++;
        }
    } else {
        select_random_number_with_index_seed(left, right, num_neighbor, index_storage, *total_offsets, seed);
        *total_offsets += num_neighbor;
    }

    // return 1 if the target has any neighbor
    return (*total_offsets > 0) ? 1 : 0;
}

/*
    ring - iouring ring structure
    index_storage - an index with all selected neighbor offsets stored
    target_storage - storage stores the targets nodes that need to be read
    target_num - number of target nodes in the target_storage 
    QD: queue depth, which defines the SQ and CQ size
    infd - file description 
    neighbor_storage - storage that store all the return neighbors
    neighbor_index - neighbor storage pointer
    data_storage - a storage that stores pre-allocated io_data structure
    all_offsets_storage - array that stores the whole offset files, where we can read the neighbor offsets of target nodes
    This is a improved async reading and store function, in which we overlap the getting offsets and waiting for completion steps. 
    However, the deduplicating process is not included here. Instead, we do it in the main functon
*/
int async_read_and_store_with_duplicates(struct io_uring *ring, uint64_t *index_storage, uint32_t *target_storage, int target_num, int QD, int infd, uint32_t *neighbor_storage, int neighbor_index, int num_neighbor, struct io_data *data_storage, uint64_t *all_offsets_storage, unsigned int *seed) {
    // printf("async_read_and_store_with_duplicates called\n");
    int total_offsets = 0; // keep track of the number offsets in the storage
    int sqe_num = 0; // keep track of the read rerquest number we submitted for the SQ
    int total_reads = 0; // keep track of total number read requests we submitted
    int batch_to_read = 0; // keep track the number of read requests within a batch 
    int submitted_offsets = 0; // keep track of the number of offsets we submtted in this batch 
    int complete_targets = 0;// keep track of the number of on targeted that have
    int batch_size = QD / num_neighbor; // the number of target nodes each batch has
    int target_to_read = 0; // exact amount of target we read in each batch 
    int max_neighbor_num = target_num * num_neighbor; // the maximum number of neighbors there might be 
    struct io_data *data;
    int data_index = 0;
    int ret; 
    
    // see how many targets we can read
    // if the targets to read left is fewer than batch size, we simply read all of them
    // otherwise, we process batch_size amount of target
    if ((complete_targets + batch_size) < target_num) {
        target_to_read = batch_size;
    } else {
        target_to_read = target_num - complete_targets;
    }

    // process offsets for the very first batch of target nodes and store the offsets in the index storage
    for (int i = 0; i < target_to_read; i++) {
       if (complete_targets >= target_num) {
            break;
        }
        // if a target has no neighbors, we move on to read the next target,
        // until we finish processing all targets or meet any target that has neighbors
        while (complete_targets < target_num && !get_and_store_offsets(target_storage[complete_targets], all_offsets_storage, index_storage, &total_offsets, num_neighbor, seed)) {
            complete_targets++;
        }
        // Increment complete_targets after successfully processing a target
        complete_targets++;
    }

    total_reads += total_offsets;
    batch_to_read = total_offsets;

    // submit the offsets for the first batch 
    while (sqe_num < QD && submitted_offsets < total_offsets) {
        uint64_t offset = index_storage[submitted_offsets] * sizeof(int);
        data = &data_storage[data_index];
        if (queue_read(ring, sizeof(int), offset, infd, data)) {
            printf("Error creating SQE for offset %jd\n", offset);
            return 1;
        }
        data_index++;
        sqe_num++;
        submitted_offsets++;
    }
    // submit the first SQ ring
    ret = io_uring_submit(ring);
    if (ret < 0) {
        fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
        return 1;
    }
    sqe_num = 0;
    submitted_offsets = 0;

    // keep looping when we still have incompleted reading requests
    while (batch_to_read > 0) {
        total_offsets = 0;

        // process offsets of a batch of targets before we wait for completion completion 
        if ((complete_targets + batch_size) < target_num) {
            target_to_read = batch_size;
        } else {
            target_to_read = target_num - complete_targets;
        }

        for (int i = 0; i < target_to_read; i++) {
            if (complete_targets >= target_num) {
                break;
            }
            // if a target has no neighbors, we move on to read the next target,
            // until we finish processing all targets or meet any target that has neighbors
            while (complete_targets < target_num && !get_and_store_offsets(target_storage[complete_targets], all_offsets_storage, index_storage, &total_offsets, num_neighbor, seed)) {
                complete_targets++;
            }
            // Increment complete_targets after successfully processing a target
            complete_targets++;
        }
        total_reads += total_offsets;

        // submit the offsets of this batch 
        while (sqe_num < QD && submitted_offsets < total_offsets) {
            uint64_t offset = index_storage[submitted_offsets] * sizeof(int);
            // if (queue_read_malloc(ring, sizeof(int), offset, infd)) {
            data = &data_storage[data_index];
            if (queue_read(ring, sizeof(int), offset, infd, data)) {
                printf("Error creating SQE for offset %jd\n", offset);
                return 1;
            }
            data_index++;
            sqe_num++;
            submitted_offsets++;
        }
        // submit the first SQ ring
        ret = io_uring_submit(ring);
        if (ret < 0) {
            fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
            return 1;
        }
        sqe_num = 0;
        submitted_offsets = 0;

        // time how long it takes to get a batch of completion (get a CQ)
        while (batch_to_read > 0) {
            // check if there is any completion entry
            if (process_completion(ring, neighbor_storage, &neighbor_index, &batch_to_read)) {
                return 1; // Handle error
            }
        }
        batch_to_read = total_offsets;
    }
    
    // if there are some unused spacse in the neighbor_storage, we fill them with 0s
    // just in case there is any neighbor value from the last batch is not overwritten 
    int num_to_fill = max_neighbor_num - total_reads; 
    if (num_to_fill > 0) {
        memset(&neighbor_storage[total_reads] , 0, num_to_fill * sizeof(int));
    }

    return total_reads;
}

/*
    ring - the ring structure
    neighbor_storage - storage where the neighbors are stored
    neighbor_index - counter to keep track of the neighbor numbers in the neighbor storage
    batch_to_read - the numbers of reads in the completion 
    This a helper function called by the main reading function to wait of the completions of a CQ
    By calling this function, we can get the reutnr of a CQE, and store the value to the neighbor storage
    Batch_to_read variable is to keep track of how many reads left we need to wait
*/
int process_completion(struct io_uring *ring, uint32_t *neighbor_storage, int *neighbor_index, int *batch_to_read) {
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

    uint32_t *int_array = (uint32_t *)data->iov.iov_base;
    neighbor_storage[*neighbor_index] = int_array[0];
    (*neighbor_index)++;
    uint64_t current_offset = data->offset;
    // printf("offset: %ld, neighbor value: %u\n", current_offset/4, int_array[0]);

    io_uring_cqe_seen(ring, cqe);
    (*batch_to_read)--;

    return 0;
}

void* process_batch_multi_epoch(void *arg) {
    // read variables
    struct BatchArgs *threadArgs = (struct BatchArgs *)arg;
    int thread_id = threadArgs->thread_id;
    int QD = threadArgs->QD;
    int num_threads = threadArgs->num_threads;
    long long int total_reads = 0;
    uint32_t *all_train_nodes = threadArgs->all_train_nodes;
    int64_t *all_offsets_storage = threadArgs->all_offsets_storage;
    int start_index = threadArgs->start_index;
    int num_batches = threadArgs->num_batches;
    uint32_t *target_storage = threadArgs->target_storage;
    uint64_t *index_storage = threadArgs->index_storage;
    uint32_t *neighbor_storage1 = threadArgs->neighbor_storage1;
    uint32_t *neighbor_storage2 = threadArgs->neighbor_storage2;
    uint32_t *neighbor_storage3 = threadArgs->neighbor_storage3;
    struct io_data *data_storage = threadArgs->data_storage;
    int epoch_num = threadArgs->epoch_num;
    pthread_barrier_t *epoch_barrier = threadArgs->epoch_barrier;
    pthread_barrier_t *shuffle_barrier = threadArgs->shuffle_barrier;
    char *edge_path = threadArgs->edge_path;
    int BATCH_SIZE = threadArgs->batch_size;
    int training_nodes = threadArgs->training_nodes;
    unsigned int seed = time(NULL) ^ thread_id;

    uint32_t *neighbor_storages[] = {neighbor_storage1, neighbor_storage2, neighbor_storage3};

    // create file descripter
    int infd = open(edge_path, O_RDONLY);
    if (infd < 0) {
        printf("Error opening the file: %s\n", edge_path);
        pthread_exit((void *)(intptr_t)0);
    }

    // initialize the rings 
    struct io_uring batch_ring;
    if (setup_context(QD, &batch_ring)) {
        return NULL;
        pthread_exit((void *)(intptr_t)0);
    }

    for (int epoch = 0; epoch < epoch_num; epoch++) {
        // process batches assigned to this thread
        pthread_barrier_wait(shuffle_barrier);
        if (thread_id == 0) {
            printf("work on epoch: %d\n", epoch);
        }
        // start timing 
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        for(int batch = 0; batch < num_batches; batch ++) {
            int current_batch_size = BATCH_SIZE;
            if (start_index + batch * BATCH_SIZE + BATCH_SIZE > training_nodes) {
                current_batch_size = training_nodes - (start_index + batch * BATCH_SIZE);
            }
            memcpy(target_storage, all_train_nodes + start_index + batch * BATCH_SIZE, current_batch_size * sizeof(uint32_t));
            int target_num = current_batch_size;
            long long int batch_reads = 0;
            
            for (int hop = 0; hop < 3; hop ++) {
                int num_neighbor = neighbor_size[hop];

                // call iouring async reading function (return contains duplicates)
                int neighbor_num = async_read_and_store_with_duplicates(&batch_ring, index_storage, target_storage, target_num, QD, infd, neighbor_storages[hop], 0, num_neighbor, data_storage, all_offsets_storage, &seed);
                
                // remove the duplication of the return neighbors of this hop and store
                quickSort(neighbor_storages[hop], 0, neighbor_num - 1);
                removeDuplicatesAndCopy(neighbor_storages[hop], &neighbor_num, target_storage, &target_num);
                batch_reads += target_num;
                total_reads += target_num;
            }
            // printf("batch_reads: %lld\n", batch_reads);
        }
        pthread_barrier_wait(epoch_barrier);

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        long long elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
        double elapsed_time_seconds = (double)elapsed_time_ns / 1000000000;
        if (thread_id == 0) {
            printf("runtime of epoch %d is : %f\n", epoch, elapsed_time_seconds);
        }
    }
    pthread_exit((void *)(intptr_t)total_reads);
}
