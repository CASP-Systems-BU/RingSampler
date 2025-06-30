#include <stdio.h>
#include <string.h>

// suppose you finished your work
// use this file to demostrate how others using your library

int main(){

    // create a sampler instance
    // Sampler sampler = new Sampler();

    // load in edge file
    // sampler.load_edge_file("path/to/edge/file");

    // get the sampling of a node id 15 with hop 3 and sampling size 100
    // my_sample = sampler.get_sampling(15, 3, 100);

    // std::cout << my_sample << std::endl;
    
    
    // above is only a demo, you can define your own interface

    return 0;
}


// below is the code from preivious coreFunction.c
// I think you can modify it to
/*
int main(int argc, char *argv[]) {
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [file name] <[file name] ...>\n", argv[0]);
        return 1;
    }

    char *test_nodes = "/home/cc/yahoo/test_nodes.bin";
    FILE* target_file = fopen(test_nodes, "rb");
    if (target_file == NULL) {   
        printf("Error opening the file: %s\n", test_nodes);
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

    // select 1000 nodes from the target_set file 
    int target_size = 1000;
    // Calculate the number of integers in the file
    size_t initial_total_nodes = target_file_size / sizeof(int);

    // get the index range of random read
    int range_left = 0;
    int range_right = (int)initial_total_nodes;

    if (target_size <= 0 || target_size > (range_right - range_left + 1)) {
        printf("Invalid number of random numbers to select (target_size).\n");
        fclose(target_file);
        return 1;
    }

    // the maximum number of targets we can have
    int space_size = 1000 * 20 * 15 * 10 + 1000;
    // store the target nodes
    int *target_storage = (int *)malloc(space_size * sizeof(int));
    int *index_storage = (int *)malloc(target_size * sizeof(int));
    if (target_storage == NULL || index_storage == NULL) {
        perror("malloc");
        free(target_storage);
        free(index_storage);
        return 1;
    }

    // Seed the random number generator with the current time
    srand(time(NULL));
    // Call the function to select random numbers and store them in randomNumbers
    selectRandomNumbers(range_left, range_right, target_size, index_storage);

    fclose(target_file);

    dump_List(index_storage, target_size);

    
    // After have the random 1000 target nodes stroed in the target_storage,
    // we now use io_uring to get their neighbors in the edges file
    
    // keep track of the number of neighbors we find in total 
    int neighbors_added = 0;
    // for each target node in the target_storage
    for (int target=0; target<target_size; target++) {
        struct io_uring ring;
        int ret;

        // initiate io_uring
        io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

        for (int i = 0; i < target_size; i++) {
            ret = submit_read_request(argv[1], &ring, index_storage[i]);
            if (ret) {
                fprintf(stderr, "Error reading file: %s\n", argv[1]);
                return 1;
            }
            get_completion_and_store(&ring, target_storage, i);
        }
        io_uring_queue_exit(&ring);
    }

    dump_List(target_storage, target_size);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long long elapsed_time_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
    double elapsed_time_seconds = (double)elapsed_time_ns / 1000000000;
    double elapsed_time_ms = (double)elapsed_time_ns / 1000000;

    printf("Time taken: %f milliseconds\n", elapsed_time_ms);
    printf("Time taken: %f seconds\n", elapsed_time_seconds);

    free(target_storage);
    free(index_storage);

    return 0;
}
*/