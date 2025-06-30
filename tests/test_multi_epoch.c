/*
    For the final experiments, there will be more than 1 epoches to process. 
    Based on the test_epoch_batch implementation, which process 1 epoch with multi-thread, 
    we create this implementation to process multi batches iteratively. 
    For now, we just use the train file as the target nodes of all epoches to test the functionalitys. 
*/

#include "core.h"

// this main function take the two input: QD(queue size), group size: the number of target nodes that process together 
int main(int argc, char *argv[]) {
    // take the input of QD
    if (argc < 6) {
        printf("Usage: %s <dataset> <QD> <num_threads> <epoch_num> <batch_size>\n", argv[0]);
        return 1;
    }

    // Read dataset argument
    const char *dataset = argv[1];
    char edge_path[256], offset_path[256], train_path[256];
    int training_nodes, batch_size, total_batch_num;

    // Set file paths based on the dataset
    if (strcmp(dataset, "yahoo") == 0) {
        strcpy(edge_path, YAHOO_EDGE_PATH);
        strcpy(offset_path, YAHOO_OFFSET_PATH);
        strcpy(train_path, YAHOO_TRAIN_PATH);
        training_nodes = 1400000;
    } else if (strcmp(dataset, "papers") == 0) {
        strcpy(edge_path, PAPERS_EDGE_PATH);
        strcpy(offset_path, PAPERS_OFFSET_PATH);
        strcpy(train_path, PAPERS_TRAIN_PATH);
        training_nodes = 1207179;
    } else if (strcmp(dataset, "friendster") == 0) {
        strcpy(edge_path, FRIENDSTER_EDGE_PATH);
        strcpy(offset_path, FRIENDSTER_OFFSET_PATH);
        strcpy(train_path, FRIENDSTER_TRAIN_PATH);
        training_nodes = 650000;
    } else if (strcmp(dataset, "liveJournal") == 0) {
        strcpy(edge_path, LIVE_JOURNAL_EDGE_PATH);
        strcpy(offset_path, LIVE_JOURNAL_OFFSET_PATH);
        strcpy(train_path, LIVE_JOURNAL_TRAIN_PATH);
        training_nodes = 39979;
    }  else {
        printf("Unknown dataset: %s\n", dataset);
        printf("Available datasets: yahoo, papers, friendster, liveJournal\n");
        return 1;
    }

    // Read the other inputs
    int QD = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    int epoch_num = atoi(argv[4]);
    batch_size = atoi(argv[5]);

    total_batch_num = (training_nodes + batch_size - 1) / batch_size;

    // Print paths for debugging
    printf("Using dataset: %s\n", dataset);
    printf("Batch size: %d\n", batch_size); 
    printf("Epoch num: %d\n", epoch_num); 
    printf("Thread num: %d\n", num_threads); 
    printf("QD: %d\n", QD); 

     // read train_nodes file
    FILE* train_file = fopen(train_path, "rb");
    if (train_file == NULL) {   
        printf("Error opening the file: %s\n", train_path);
        fclose(train_file);
        return 1;
    }

    // store all train nodes in the storage
    uint32_t* all_train_nodes = malloc(training_nodes * sizeof(uint32_t));
    size_t items_read = fread(all_train_nodes, sizeof(uint32_t), training_nodes, train_file); 
    fclose(train_file);
    if (items_read != training_nodes) {
        printf("Error reading from file: %s\n", train_path);
        fclose(train_file);
        return 1;
    } 

    // Open offset file
    FILE *offset_file = fopen(offset_path, "rb");
    if (offset_file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Determine offset file size
    fseek(offset_file, 0, SEEK_END);
    long file_size = ftell(offset_file);
    rewind(offset_file);

    // Calculate the number of offsets (assuming each offset is of type int64_t)
    int num_offsets = file_size / sizeof(int64_t);

    // Allocate memory to store the offsets
    int64_t *all_offsets_storage = (int64_t *)malloc(num_offsets * sizeof(int64_t));
    if (all_offsets_storage == NULL) {
        perror("Memory allocation failed");
        fclose(offset_file);
        return 1;
    }

    // Read the offsets from the offset file into the storage
    size_t elements_read = fread(all_offsets_storage, sizeof(int64_t), num_offsets, offset_file);
    if (elements_read != num_offsets) {
        perror("Error reading file");
        fclose(offset_file);
        free(all_offsets_storage);
        return 1;
    }
    fclose(offset_file);

    // initilize threads and barrier
    pthread_t threads[num_threads];
    struct BatchArgs thread_args[num_threads];
    pthread_barrier_t epoch_barrier;
    pthread_barrier_t shuffle_barrier;
    pthread_barrier_init(&epoch_barrier, NULL, num_threads+1); 
    pthread_barrier_init(&shuffle_barrier, NULL, num_threads+1); 

    // Preallocate memory for all threads
    int space_size = batch_size * 20 * 15 * 10;
    uint32_t **target_storage_array = malloc(num_threads * sizeof(uint32_t *));
    uint64_t **index_storage_array = malloc(num_threads * sizeof(uint64_t *));
    uint32_t **neighbor_storage1_array = malloc(num_threads * sizeof(uint32_t *));
    uint32_t **neighbor_storage2_array = malloc(num_threads * sizeof(uint32_t *));
    uint32_t **neighbor_storage3_array = malloc(num_threads * sizeof(uint32_t *));
    struct io_data **data_storage_array = malloc(num_threads * sizeof(struct io_data *));

    for (int i = 0; i < num_threads; i++) {
        target_storage_array[i] = malloc((space_size+batch_size) * sizeof(uint32_t));
        index_storage_array[i] = malloc((space_size+batch_size) * sizeof(uint64_t));
        neighbor_storage1_array[i] = malloc(space_size / 15 / 10 * sizeof(uint32_t));
        neighbor_storage2_array[i] = malloc(space_size / 10 * sizeof(uint32_t));
        neighbor_storage3_array[i] = malloc(space_size / 5 * sizeof(uint32_t));
        data_storage_array[i] = malloc((space_size+batch_size) * (sizeof(uint32_t) + sizeof(struct io_data)));
        for (int j = 0; j < (space_size+batch_size); j++) {
            data_storage_array[i][j].iov.iov_base = malloc(sizeof(uint32_t));
        }
    }

    // Calculate the number of batches each thread should process
    int batches_per_thread = total_batch_num / num_threads;
    int remainder_batches = total_batch_num % num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].num_threads = num_threads;
        thread_args[i].QD = QD;
        thread_args[i].all_train_nodes = all_train_nodes;
        thread_args[i].all_offsets_storage = all_offsets_storage;
        thread_args[i].start_index = i * batches_per_thread + (i < remainder_batches ? i : remainder_batches);
        thread_args[i].num_batches = batches_per_thread + (i < remainder_batches ? 1 : 0);
        thread_args[i].target_storage = target_storage_array[i];
        thread_args[i].index_storage = index_storage_array[i];
        thread_args[i].neighbor_storage1 = neighbor_storage1_array[i];
        thread_args[i].neighbor_storage2 = neighbor_storage2_array[i];
        thread_args[i].neighbor_storage3 = neighbor_storage3_array[i];
        thread_args[i].data_storage = data_storage_array[i];
        thread_args[i].epoch_barrier = &epoch_barrier;
        thread_args[i].shuffle_barrier = &shuffle_barrier;
        thread_args[i].epoch_num = epoch_num;
        thread_args[i].edge_path = edge_path;
        thread_args[i].train_path = train_path;
        thread_args[i].batch_size = batch_size;
        thread_args[i].training_nodes = training_nodes;

        if (pthread_create(&threads[i], NULL, process_batch_multi_epoch, &thread_args[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int epoch = 0; epoch < epoch_num; epoch++) {
        printf("shuffle epoch: %d\n", epoch);
        shuffle(all_train_nodes, training_nodes);
        pthread_barrier_wait(&shuffle_barrier);
        pthread_barrier_wait(&epoch_barrier); // Wait for threads to finish processing epoch
    }

    // wait for the threads finish processing batch
    long long int total_reads = 0;
    void *retVal;
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], &retVal) != 0) {
            perror("pthread_join");
        } else {
            total_reads += (long long int)retVal;
        }
    }

    free(all_train_nodes);
    free(all_offsets_storage);
    close(infd);

    return 0;
}