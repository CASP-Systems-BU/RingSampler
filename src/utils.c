#include "utils.h"

/*
arr - array of integers that needs to be shuffled
n - length of the array
Shuffle the array of integers randomly
*/
void shuffle(int *arr, int n) {
    // Seed the random number generator
    srand(time(NULL));

    for (int i = n - 1; i > 0; i--) {
        // Generate a random index between 0 and i (inclusive)
        int j = rand() % (i + 1);

        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


/*
list - array of offsets(data type uint64_t)
size - lengh of the array
print offset array
*/
void dump_List_offset(const uint64_t *list, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%ld", list[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

/* 
fd - file description
Returns the size of the file 
*/
off_t get_file_size(int fd) {
    struct stat st;
    if(fstat(fd, &st) < 0) {
        perror("fstat");
        return -1;
    }

    // If it's a block device, use ioctl to get the size.
    if (S_ISBLK(st.st_mode)) {
        unsigned long long bytes;
        if (ioctl(fd, BLKGETSIZE64, &bytes) != 0) {
            perror("ioctl");
            return -1;
        }
        return bytes;
    // If it's a regular file, return the size from st struct.
    } else if (S_ISREG(st.st_mode))
        return st.st_size;

    // Return -1 for other types of files.
    return -1;
}


/* 
list - array of integers
size - length of the array
print the array of integers
*/
void dump_List(const uint32_t *list, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%u", list[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

/*
entries - queue depth. The size of the rings
ring - ring structure
initialization SQ and CQ
*/
int setup_context(unsigned entries, struct io_uring *ring) { // entries is QD, the size of SQ. CQ size is 2*QD
    int ret;

    ret = io_uring_queue_init(entries, ring, 0); // Initialize io_uring
    if( ret < 0) {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }

    return 0;
}

double calculate_elapsed_time_ms(struct timespec start, struct timespec end) {
    long long elapsed_time_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    return (double)elapsed_time_ns / 1000000;
}