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
#define NUM_THREADS 100         // Number of threads for concurrency test

void *stress_test_thread() {
    void *ptrs[ALLOC_TEST_SIZE] = {NULL};
    unsigned int thread_seed = time(NULL) ^ (unsigned long)pthread_self();
    srand(thread_seed);

    for (size_t i = 0; i < ALLOC_TEST_SIZE; i++) {
        size_t size = rand() % MAX_ALLOC_SIZE + 1;
        ptrs[i] = ft_malloc(size);
      //  printf("Thread %lu malloc\n", (unsigned long)pthread_self());
        if (ptrs[i] == NULL) {
       //     printf("ft_malloc failed for size %zu\n", size);
            continue;
        }
        
        // Randomly decide to ft_realloc
        if (rand() % 2 == 0) {
            size_t new_size = rand() % MAX_ALLOC_SIZE + 1;
            void *new_ptr = ft_realloc(ptrs[i], new_size);
        //	printf("Thread %lu realloc\n", (unsigned long)pthread_self());
            if (new_ptr == NULL) {
         //       printf("ft_realloc failed for size %zu\n", new_size);
            } else {
                ptrs[i] = new_ptr;
                size = new_size; // Update size after successful reallocation
            }
        }
        
        // Write data to memory
        memset(ptrs[i], i % 255, size); // Fill with some data
      //  printf("Thread %lu: write %zu bytes\n", (unsigned long)pthread_self(), size);
        
        // Randomly ft_free half the allocations
        if (rand() % 2 == 0) {
            ft_free(ptrs[i]);
      //  	printf("Thread %lu free\n", (unsigned long)pthread_self());
            ptrs[i] = NULL;
        }
    }
    
    // Free remaining allocations
    for (size_t i = 0; i < ALLOC_TEST_SIZE; i++) {
        if (ptrs[i] != NULL) { ft_free(ptrs[i]);
        }
    }
    return NULL;
}

void run_concurrency_test() {
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, stress_test_thread, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
    }
}

int main() {
    srand(time(NULL));
//	printf("Starting single-threaded stress test...\n");
    
    // Single-threaded stress test
 //   stress_test_thread(NULL);
//	printf("test1 ok\n");
 //  	printf("Starting single-threaded stress test...\n");
//    stress_test_thread(NULL);
    
    printf("Running multi-threaded stress test...\n");
    run_concurrency_test();
    
    printf("Stress test completed.\n");
    return 0;
}
