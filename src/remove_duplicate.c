/*
These functions are used after we finish sampling each hop
The general idea of removing duplicatiom is 
    - sort all the neighbor nodes we collect by using quick sort
    - loop throught the sorted neighbors to remove duplication
    - meanwhile, we will keep track of the number of the neighbors without duplication 
*/

#include "remove_duplicate.h"

/* 
a - integer 1
b - integer 2
This function acts as a helper function of quickSort.
It will swap integers a and b
*/
void swap(uint32_t* a, uint32_t* b) {
    uint32_t t = *a;
    *a = *b;
    *b = t;
}

/* 
arr - an array of integers
low - left end of the array
high - right end of the array
This function acts as a helper function of quickSort.
Implement parition algorithm
*/
int partition(uint32_t arr[], int low, int high) {
    uint32_t pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/*
a - pointers to constant void 1
b - pointers to constant void 2
A comparator for qusort function to compare 
returns a negative value; if they are equal, it returns zero; and if a should come after b, it returns a positive value.
*/
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

/*
arr -  array of integers
size - lengh of array
use qsort to sort the array of integers
*/
void sortArray(int* arr, int size) {
    qsort(arr, size, sizeof(int), compare);
}

/* 
arr - array of integers
low - left end of the array 
hgih - right end of the array 
This function sort an array by using quickSort and partition, which doesn't take extra memories
*/
void quickSort(uint32_t* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/* 
sortedList - an array of integers that has been sorted
sortedLength - the length of the sorted list.
emptyList - an empty array where we store the deduplicated sortedList
emptyLength - keep track of the number of integers stored in the emptyList
This function can remove the duplicates, and copy the new unique list to an empty list.
*/
void removeDuplicatesAndCopy(uint32_t* sortedList, uint32_t* sortedLength, uint32_t* emptyList, int* emptyLength) {
    if (*sortedLength <= 0) {
        return; // If the sorted list is empty or has negative length, return immediately
    }

    // Copy the first element from the sorted list to the empty list
    emptyList[0] = sortedList[0];
    *emptyLength = 1;

    // Iterate through the sorted list
    for (int i = 1; i < *sortedLength; i++) {
        // If the current element is different from the previous one, copy it to the empty list
        if (sortedList[i] != sortedList[i - 1]) {
            emptyList[*emptyLength] = sortedList[i];
            (*emptyLength)++;
        }
    }

    // Set the length of the sorted list to 0
    *sortedLength = 0;
}

int filter_zeros(uint32_t *neighbor_storage, int neighbor_num) {
    int valid_neighbors = 0;
    for (int i = 0; i < neighbor_num; i++) {
        if (neighbor_storage[i] != 0) {
            neighbor_storage[valid_neighbors++] = neighbor_storage[i];
        }
    }
    return valid_neighbors;
}