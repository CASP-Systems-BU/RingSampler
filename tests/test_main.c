/*
With this file, you have the flexibility to adjust all variables of io_uring sampling, including
- input files: edge file, offset file, target file 
- Polling mode: general polling, kernel polling 
- Reading method: asynch, synch 
- hop and hop size: 20 15 10...s
*/

#include "core.h"

int main() {
    // create all variables
    char edgeFile[256];
    char offsetFile[256];
    char targetFile[256];
    int pollingMode;
    int readingMethod;
    int SQ_size;
    int CQ_size;
    int *hopList;
    int numHopSizes;
    // collect values of the variables 

    // // input printer for testing
    // printf("Edge File: %s\n", edgeFile);
    // printf("Offset File: %s\n", offsetFile);
    // printf("Target File: %s\n", targetFile);
    // printf("Polling Mode: %d\n", pollingMode);
    // printf("Reading Method: %d\n", readingMethod);
    // printf("SQ size: %d\n", SQ_size);
    // printf("CQ size: %d\n", CQ_size);
    // printf("Hop Size List: ");
    // for (int i = 0; i < numHopSizes; i++) {
    //     printf("%d ", hopList[i]);
    // }
    // printf("\n");

    // free(hopList);

    // read queue depth
    // int QD = SQ_size;
    int QD = 20;

    FILE* target_file = fopen(targetFile, "rb");
    if (target_file == NULL) {   
        printf("Error opening the file: %s\n", targetFile);
        return 1;
    }

    int infd = open(edgeFile, O_RDONLY);
    if (infd < 0) {
        printf("Error opening the file: %s\n", edgeFile);
        return 1;
    }

    // Determine the size of the target_file
    fseek(target_file, 0, SEEK_END);
    long target_file_size = ftell(target_file);
    fseek(target_file, 0, SEEK_SET);

    if (target_file_size <= 0) {
        printf("File is empty or couldn't determine its size.\n");
        fclose(target_file);
        return 1;
    }

    // initialize the variables and storages 
    int total_nodes = 146818;         
    int neighbors_added;
    int space_size = 1;
    for (int i = 0; i < numHopSizes; i++) {
        space_size *= hopList[i];
    }
    space_size += 1000;
    int final_num = 1000;
    // storages
    int *target_storage = (int *)malloc(space_size * sizeof(int));
    uint64_t *index_storage = (uint64_t *)malloc(space_size * sizeof(uint64_t));
    int *neighbor_storage = (int *)malloc(space_size * sizeof(int));
    int *final_storage = (int *)malloc(space_size * sizeof(int));
    struct io_data *data = malloc(sizeof(*data) + sizeof(int));
    data->iov.iov_base = malloc(sizeof(int));
    if (target_storage == NULL || index_storage == NULL || neighbor_storage == NULL || final_storage == NULL) {
        perror("malloc");
        free(target_storage);
        free(index_storage);
        free(neighbor_storage);
        free(final_storage);
        return 1;
    }

    // select random 1000 nodes from the target file
    int indices[1000];
    int usedIndices[146818] = {0};
    srand(time(NULL));
    for (int i = 0; i < 1000; i++) {
        int randomIndex;
        do {
            randomIndex = rand() % total_nodes;
        } while (usedIndices[randomIndex]); // Ensure uniqueness

        indices[i] = randomIndex;
        usedIndices[randomIndex] = 1; // Mark as used
    }

    // Use generated indices to access integers in the .bin file and store them in both the final_sampling_result and the workspace_for_each_hop
    int target_num = 0;

    // initialize io_uring
    struct io_uring ring;
    off_t insize;
    int ret;

    if (setup_context(QD, &ring))
        return 1;
 
    // start timing 
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

   for (int hop = 0; hop < 3; hop ++) {
        // int num_neighbor = neighbor_size[hop];
        int num_neighbor = 10;
        neighbors_added = 0;
        for (int target = 0; target < target_num; target++) {
            // TODO: remove readTuples. The core of the function needs to be updated
            // struct tuple tuples = readTuples(target_storage[target]);
            uint64_t left = 0;
            uint64_t right = 10;
            int read_num = 0;

            int current_read = right - left; 
            if (right - left <= num_neighbor) {
                // memcpy(index_storage, &left, (right - left) * sizeof(uint64_t));
                for (uint64_t i = left; i < right; i++) {
                    index_storage[i-left] = i;
                }
            } else {
                selectRandomNumbers(left, right, num_neighbor, index_storage);
                current_read = num_neighbor;
            }
            while (read_num < current_read) {
                size_t remaining_reads = current_read - read_num;
                size_t current_batch = remaining_reads > QD ? QD : remaining_reads;
                // TODO: this reading function is outdated
                // ret = synch_read_file_at_indexes_and_store(&ring, &index_storage[read_num], current_batch, neighbor_storage, neighbors_added, infd);
                if (ret != 0) {
                    break;
                }
                read_num += current_batch;
                neighbors_added += current_batch;
            }
        }
        quickSort(neighbor_storage, 0, neighbors_added - 1);
        removeDuplicatesAndCopy(neighbor_storage, &neighbors_added, target_storage, &target_num);
        memcpy(final_storage + final_num, target_storage, target_num * sizeof(int));
        final_num += target_num;
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long long elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
    double elapsed_time_seconds = (double)elapsed_time_ns / 1000000000;
    double elapsed_time_ms = (double)elapsed_time_ns / 1000000;

    printf(" %f \n", elapsed_time_ms);
    // printf("Time taken: %f seconds\n", elapsed_time_seconds);

    close(infd);
    fclose(target_file);
    free(target_storage);
    free(index_storage);
    free(neighbor_storage);
    free(final_storage);
    io_uring_queue_exit(&ring);
    return ret;
    // return 0;
}