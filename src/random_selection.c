/*
This function open a node file, and randomly select certain number of indexes
within a given range and store it in the storage. 
*/
#include "random_selection.h"

/*
left - left end of the range
right - right end of the range
target_size - number of random nubers we select in total 
index_storage - where we store the selected numbers
randomly select target_size number of integers between the range of [left, right],
and store them in the index_storage
*/
void selectRandomNumbers(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage) {
    for (int i = 0; i < target_size; i++) {
        index_storage[i] = rand() % (right - left + 1) + left;
    }
}

/*
left - left end of the range
right - right end of the range
target_size - number of random nubers we select in total 
index_storage - where we store the selected numbers
index - index where we start to store the the first number
randomly select target_size number of integers between the range of [left, right],
and store them in the index_storage.
Moreover, this function allows to specify where to store the integers in the index storage
*/
void select_random_number_with_index(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage, int index) {
    for (int i = 0; i < target_size; i++) {
        index_storage[i+index] = rand() % (right - left + 1) + left;
    }
}

void select_random_number_with_index_seed(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage, int index, unsigned int *seed) {
    for (int i = 0; i < target_size; i++) {
        index_storage[i + index] = rand_r(seed) % (right - left) + left;
    }
}
