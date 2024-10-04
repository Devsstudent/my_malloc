#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include "ft_malloc.h"

// Function prototypes for your custom allocator
void *ft_malloc(size_t size);
void *ft_realloc(void *ptr, size_t size);
void ft_free(void *ptr);

// Allocator stress test configuration
#define ALLOC_TEST_SIZE 10000 // Number of allocations to perform
#define MAX_ALLOC_SIZE 1024   // Maximum size of allocation in bytes
#define NUM_THREADS 0         // Number of threads for concurrency test

void *stress_test_thread(void *arg) {
    void *ptrs[ALLOC_TEST_SIZE] = {NULL};
    for (int i = 0; i < ALLOC_TEST_SIZE; i++) {
        size_t size = rand() % MAX_ALLOC_SIZE + 1;
        ptrs[i] = ft_malloc(size);
        
        // Randomly decide to ft_realloc
        if (rand() % 2 == 0 && ptrs[i] != NULL) {
            size_t new_size = rand() % MAX_ALLOC_SIZE + 1;
            ptrs[i] = ft_realloc(ptrs[i], new_size);
        }
        
        // Optionally, write data to memory
        if (ptrs[i] != NULL) {
            memset(ptrs[i], i % 255, size); // Fill with some data
        }
        
        // Randomly ft_free half the allocations
        if (rand() % 2 == 0 && ptrs[i] != NULL) {
            ft_free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }
    
    // Free remaining allocations
    for (int i = 0; i < ALLOC_TEST_SIZE; i++) {
        if (ptrs[i] != NULL) {
            ft_free(ptrs[i]);
        }
    }

    return NULL;
}

void run_concurrency_test() {
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stress_test_thread, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    srand(time(NULL));

    printf("Starting stress test...\n");
    
    // Single-threaded stress test
    stress_test_thread(NULL);

    // Multithreaded stress test
    printf("Running multi-threaded stress test...\n");
    run_concurrency_test();

    printf("Stress test completed.\n");
    return 0;
}
