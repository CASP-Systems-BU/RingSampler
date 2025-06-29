#ifndef RANDOM_SELECTION_H
#define RANDOM_SELECTION_H

#include <stdint.h>
#include <stdlib.h>

void selectRandomNumbers(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage);
void select_random_number_with_index(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage, int index);
void select_random_number_with_index_seed(uint64_t left, uint64_t right, int target_size, uint64_t *index_storage, int index, unsigned int *seed);

#endif // RANDOM_SELECTION_H