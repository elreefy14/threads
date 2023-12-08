//input file name
// /home/elreefy14/Desktop/6701-matmult/input-sort.txt
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int* arr;
    int start;
    int end;
} ThreadArgs;

void merge(int* arr, int start, int mid, int end) {
    // Calculate the sizes of the left and right subarrays
    int leftSize = mid - start + 1;
    int rightSize = end - mid;

    // Allocate memory for temporary arrays to store the left and right subarrays
    int* leftArr = (int*)malloc(leftSize * sizeof(int));
    int* rightArr = (int*)malloc(rightSize * sizeof(int));

    // Copy elements from the original array to the left and right subarrays
    for (int i = 0; i < leftSize; i++) {
        leftArr[i] = arr[start + i];
    }

    for (int i = 0; i < rightSize; i++) {
        rightArr[i] = arr[mid + 1 + i];
    }

    // Merge the left and right subarrays back into the original array
    int i = 0;
    int j = 0;
    int k = start;

    while (i < leftSize && j < rightSize) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy any remaining elements from the left subarray
    while (i < leftSize) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy any remaining elements from the right subarray
    while (j < rightSize) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }

    // Free the memory allocated for the temporary arrays
    free(leftArr);
    free(rightArr);
}

void* mergeSort(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int* arr = args->arr;
    int start = args->start;
    int end = args->end;

    // Base case: If the start index is less than the end index, perform merge sort
    if (start < end) {
        // Calculate the mid index
        int mid = start + (end - start) / 2;

        // Create ThreadArgs structures for the left and right subarrays
        ThreadArgs leftArgs = {arr, start, mid};
        ThreadArgs rightArgs = {arr, mid + 1, end};

        // Create threads to sort the left and right subarrays
        pthread_t leftThread, rightThread;
        pthread_create(&leftThread, NULL, mergeSort, (void*)&leftArgs);
        pthread_create(&rightThread, NULL, mergeSort, (void*)&rightArgs);

        // Wait for the threads to complete
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        // Merge the sorted left and right subarrays
        merge(arr, start, mid, end);
    }

    return NULL;
}

int main() {
    char filename[100];
    printf("Enter the input file name: ");
    scanf("%s", filename);

    // Open the input file
    FILE* file = fopen(filename, "r");

    // Check if the file failed to open
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    int numElements;
    fscanf(file, "%d", &numElements);

    // Allocate memory for the array
    int* arr = (int*)malloc(numElements * sizeof(int));

    // Read the elements from the file and store them in the array
    for (int i = 0; i < numElements; i++) {
        fscanf(file, "%d", &arr[i]);
    }

    // Close the file
    fclose(file);

    // Create a ThreadArgs structure for the entire array
    ThreadArgs args = {arr, 0, numElements - 1};

    // Create a thread to perform merge sort on the array
    pthread_t mergeSortThread;
    pthread_create(&mergeSortThread, NULL, mergeSort, (void*)&args);

    // Wait for the merge sort thread to complete
    pthread_join(mergeSortThread, NULL);

    // Print the sorted array
    printf("Sorted array:\n");
    for (int i = 0; i < numElements; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Free the memory allocated for the array
    free(arr);

    return 0;
}