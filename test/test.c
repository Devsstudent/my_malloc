#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>
/*
Test(mmap, simple) {
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_expect(ptr != NULL);
    int res = munmap(ptr, 4096);
    cr_expect(res == 0);
}
*/
Test(my_malloc_simple, simple) {
	printf("test simple malloc\n");
	void *ptr = my_malloc(16);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(48);
	cr_expect((size_t) ptr2 == (size_t) ptr + 16 + sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12);
}


Test(my_malloc_aligned, double_alloc_aligned_one_page) {
	printf("test multi malloc\n");
	void *ptr = my_malloc(16);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(24);
	cr_expect((size_t) ptr2 == (size_t) ptr + 16 + 1 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12);
	void *ptr3 = my_malloc(2500);
	cr_expect((size_t) ptr3 == (size_t) ptr + 16 + 24 + 2 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 25);
	my_free(ptr);
	my_free(ptr2);
	my_free(ptr3);
}

Test(my_malloc, null) {
	printf("test malloc null\n");
	void *ptr = my_malloc(0);
	cr_expect(ptr == NULL, "Failed to alloc");
}

/*
Test(my_malloc_more_pages, more_pages) {
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr1 = my_malloc(200);
	cr_expect(ptr1 != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(4000);
	cr_expect(ptr2 != NULL, "Failed to alloc");
//	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + 200 + 2 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12 + 200);
	void *ptr3 = my_malloc(400);
	cr_expect(ptr3 != NULL, "Failed to alloc ptr3");
//	cr_expect((size_t) ptr3 == (size_t) ptr + 12 + 200 + 4000 + 3 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 200 + 4000);
	void *ptr4 = my_malloc(9000);
	cr_expect(ptr4 != NULL, "Failed to alloc ptr4");
//	cr_expect((size_t) ptr4 == (size_t) ptr + 12 + 200 + 4000 + 400 + 4 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr4, (size_t)ptr + 12 + 200 + 4000 + 400);
	my_free(ptr);
	my_free(ptr1);
	my_free(ptr2);
	my_free(ptr3);
	my_free(ptr4);
}*/

Test(my_malloc_meta_a_lot, new_meta_page) {
	printf("Test new_meta_page, lot of chunk\n");
	int i = 1;
	while (i < 1000) {
		void *ptr = my_malloc(9);
		cr_expect((size_t) ptr + 10 * i + sizeof(uint64_t) * i, "Not aligned: %lx \n", (size_t)ptr);
		my_free(ptr);
		i++;
	}
	cr_expect("not crashed");
} 

Test(my_free, free_a_chunk) {
	printf("testing, to free, sould allocate at the previous place\n");
	void *ptr = my_malloc(12);
	my_free(ptr);
	void *ptr2 = my_malloc(12);
	cr_expect((size_t) ptr == (size_t) ptr2, "Free doesn't work well");
	my_free(ptr2);
	void *ptr3 = my_malloc(400);
	cr_expect((size_t) ptr == (size_t) ptr3, "Free doesn't work well");
	void *ptr4 = my_malloc(300);
	cr_expect(ptr4 != NULL, "Failed to alloc ptr4");
	void *ptr5 = my_malloc(300);
	cr_expect(ptr5 != NULL, "Failed to alloc ptr5");
	void *ptr6 = my_malloc(4067);
	cr_expect(ptr6 != NULL, "Failed to alloc ptr6");
	my_free(ptr3);
	my_free(ptr4);
	void *ptr7 = my_malloc(650);
	cr_expect(ptr7 != NULL, "Failed to alloc ptr7");
	my_free(ptr5);
	my_free(ptr7);
	my_free(ptr6);
}

Test(canary, overflow) {
	printf("test heapoveflow\n");
	void *ptr = my_malloc(12);
	char *s = (char *)ptr;
	int i = 0;
	while (i < 15) {
		s[i] = 'a';
		i++;
	}
	my_free(s);
}

Test(my_malloc, free_pages) {
	printf("test full free double arr\n");
    int rows = 1000;
    int cols = 1000;

    // Dynamically allocate memory for a 2D array of characters
    char **matrix = (char **)my_malloc(rows * sizeof(char *));
	cr_expect(matrix != NULL, "Failed to alloc matrix");

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char *)my_malloc((cols + 1) * sizeof(char)); // +1 for null terminator
		cr_expect(matrix[i] != NULL, "Failed to alloc matrix %i", i);
    }
    // Fill the 2D array with sample data
    char fillChar = 'A';
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = fillChar++;
        }
        matrix[i][cols] = '\0'; // Null-terminate each string
    }
    // Free dynamically allocated memory
    for (int i = 0; i < rows; i++) {
        my_free(matrix[i]);
    }
    my_free(matrix);
}

Test(my_calloc, well_setted_zero) {
	printf("test my calloc\n");
	void *ptr = my_calloc(sizeof(int), 10);
	cr_expect(ptr != NULL, "Failed to alloc ptr6");
	int i = 0;
	int *arr = ptr;
	while (i < 10) {
		//printf("%i\n", arr[i]);
		cr_expect(arr[i] == 0, "Error, my_calloc doesn't initilize to 0");
		i++;
	}
	my_free(ptr);
}

Test(my_realloc, realloc_bigger_block) {
	printf("test realloc\n");
	void *ptr = my_malloc(1);
	cr_expect(ptr != NULL, "Failed to alloc ptr");
	char a = 'a';
	((char *)ptr)[0] = a;
	void *reallocated_ptr = my_realloc(ptr, 15);
	cr_expect(reallocated_ptr != NULL, "Failed to realloc");
	cr_expect(((char *)reallocated_ptr)[0] == 'a', "Realloc doesn't well copy content to the new addresse");
	my_free(reallocated_ptr);
	reallocated_ptr = my_realloc(NULL, 19200);
	cr_expect(reallocated_ptr != NULL, "Failed to realloc");
	my_free(reallocated_ptr);
}

Test(my_malloc, a_lot) {
	printf("test a lot of big malloc\n");
	void *ptr = my_malloc(1200);
	cr_expect(ptr != NULL, "Failed to alloc ptr");
	void *pt2 = my_malloc(1200);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(3);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(3);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(3);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(3);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(1200);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(4097);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(12000);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
//	cr_expect((size_t) 3 + 3 + 3 + 3 + 1200 + 4097 + 12000 + 7 * sizeof(uint64_t));
	pt2 = my_malloc(12000);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(12000);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(12000);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	pt2 = my_malloc(12000);
	cr_expect(pt2 != NULL, "Failed to alloc ptr");
	my_free(ptr);
}

Test(my_realloc, relloc_lower_well_free_unsed_part) {
	void *ptr = my_malloc(1500);
	cr_expect(ptr != NULL, "Failed to alloc ptr");
	void *reallocated_ptr = my_realloc(ptr, 800);
	cr_expect(reallocated_ptr != NULL, "Failed to realloc");
	void *ptr2 = my_malloc(42);
//	cr_expect((size_t) ptr2 == (size_t) ptr + 800 + 1, "Failed to realloc");
	my_free(ptr2);
	my_free(reallocated_ptr);
}

Test(my_realloc, passing_null_ptr) {
	printf("test realloc with null ptr\n");
	void *ptr = NULL;

	void *ptr2 = my_realloc(ptr, 120);
	cr_expect(ptr2 != NULL, "Failed to alloc ptr");
	my_free(ptr2);
}

Test(my_realloc, passing_ptr_size_zero) {
	printf("test realloc with size zero\n");
	void *ptr2 = my_malloc(120);
	cr_expect(ptr2 != NULL, "Failed to alloc ptr");
	void *ptr = my_realloc(ptr2, 0);
	cr_expect(ptr == NULL, "Error realloc not freeing with size 0");
}


Test(my_malloc, stress_test) {
	printf("test stress malloc\n");
    const int N = 1000; // Nombre de répétitions
    void **ptrs = malloc(N * sizeof(void*)); // Tableau pour stocker les pointeurs alloués

    for (int i = 0; i < N; i++) {
        size_t size = (i % 10 + 1) * 10; // Taille d'allocation variable
        ptrs[i] = my_malloc(size);
        memset(ptrs[i], 0, size); // Initialise la mémoire allouée à zéro
        cr_assert_not_null(ptrs[i], "Allocation failed at iteration %d", i);
    }

    for (int i = 0; i < N; i++) {
        my_free(ptrs[i]); // Libération de la mémoire
    }

    free(ptrs); // Libérer le tableau des pointeurs
}

