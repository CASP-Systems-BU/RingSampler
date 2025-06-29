#ifndef REMOVE_DUPLICATE_H
#define REMOVE_DUPLICATE_H

#include <liburing.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"

void swap(uint32_t* a, uint32_t* b);
int partition(uint32_t arr[], int low, int high);
void quickSort(uint32_t* arr, int low, int high);
void removeDuplicatesAndCopy(uint32_t* sortedList, uint32_t* sortedLength, uint32_t* emptyList, int* emptyLength);
int compare(const void *a, const void *b);
void sortArray(int* arr, int size);
int filter_zeros(uint32_t *neighbor_storage, int neighbor_num);

#endif // REMOVE_DUPLICATE_H