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
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(25);
	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12);
}

Test(my_malloc_aligned, double_alloc_aligned_one_page) {
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(25);
	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + 1 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12);
	void *ptr3 = my_malloc(2500);
	cr_expect((size_t) ptr3 == (size_t) ptr + 12 + 25 + 2 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 25);
	void *ptr4 = my_malloc(1);
	cr_expect((size_t) ptr4 == (size_t) ptr + 12 + 25 + 2500 + 3 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr4, (size_t)ptr + 12 + 25 + 2500);
	my_free(ptr);
	my_free(ptr2);
	my_free(ptr3);
	my_free(ptr4);
}

Test(my_malloc, null) {
	void *ptr = my_malloc(0);
	cr_expect(ptr == NULL, "Failed to alloc");
}

Test(my_malloc, last_chunk_not_free) {
}
/*
Test(my_malloc_more_pages, more_pages) {
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr1 = my_malloc(200);
	cr_expect(ptr1 != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(4000);
	cr_expect(ptr2 != NULL, "Failed to alloc");
	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + 200 + 2 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12 + 200);
	void *ptr3 = my_malloc(400);
	cr_expect(ptr3 != NULL, "Failed to alloc ptr3");
	cr_expect((size_t) ptr3 == (size_t) ptr + 12 + 200 + 4000 + 3 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 200 + 4000);
	void *ptr4 = my_malloc(9000);
	cr_expect(ptr4 != NULL, "Failed to alloc ptr4");
	cr_expect((size_t) ptr4 == (size_t) ptr + 12 + 200 + 4000 + 400 + 4 * sizeof(uint64_t), "Not aligned: %lx -%lx\n", (size_t)ptr4, (size_t)ptr + 12 + 200 + 4000 + 400);
	my_free(ptr);
	my_free(ptr1);
	my_free(ptr2);
	my_free(ptr3);
	my_free(ptr4);
}*/

Test(my_malloc_meta_a_lot, new_meta_page) {
	int i = 0;
	void *ptr;
	while (i < 1000) {
		ptr = my_malloc(10);
		printf("%i\n", i);
		i++;
	}
	cr_expect("not crashed");
}
/*
Test(my_free, free_a_chunk) {
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
	cr_expect((size_t) ptr7 == (size_t) ptr3, "ptr7 %ld !=  ptr3 %ld", (size_t) ptr7, (size_t) ptr3);
	my_free(ptr5);
	my_free(ptr7);
	my_free(ptr6);
}

Test(canary, overflow) {
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
    int rows = 10;
    int cols = 5;

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
	void *ptr = my_calloc(sizeof(int), 10);
	cr_expect(ptr != NULL, "Failed to alloc ptr6");
	int i = 0;
	int *arr = ptr;
	while (i < 10) {
		printf("%i\n", arr[i]);
		cr_expect(arr[i] == 0, "Error, my_calloc doesn't initilize to 0");
		i++;
	}
	my_free(ptr);
}

Test(my_realloc, realloc_bigger_block) {
	void *ptr = my_malloc(1);
	cr_expect(ptr != NULL, "Failed to alloc ptr");
	char a = 'a';
	((char *)ptr)[0] = a;
	void *reallocated_ptr = my_realloc(ptr, 15);
	cr_expect(reallocated_ptr != NULL, "Failed to realloc");
	cr_expect(((char *)reallocated_ptr)[0] == 'a', "Realloc doesn't well copy content to the new addresse");
	my_free(reallocated_ptr);
	
}
*/
/*
Test(my_malloc, a_lot) {
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
	my_free(ptr);
}*/

/*
Test(my_realloc, relloc_lower_well_free_unsed_part) {
	void *ptr = my_malloc(1500);
	cr_expect(ptr != NULL, "Failed to alloc ptr");
	void *reallocated_ptr = my_realloc(ptr, 800);
	cr_expect(reallocated_ptr != NULL, "Failed to realloc");
	void *ptr2 = my_malloc(42);
	cr_expect((size_t) ptr2 == (size_t) ptr + 800 + 1, "Failed to realloc");
	my_free(ptr2);
	my_free(reallocated_ptr);
}

Test(my_realloc, passing_null_ptr) {
	void *ptr = NULL;

	void *ptr2 = my_realloc(ptr, 120);
	cr_expect(ptr2 != NULL, "Failed to alloc ptr");
	my_free(ptr2);
}

Test(my_realloc, passing_ptr_size_zero) {
	void *ptr2 = my_malloc(120);
	cr_expect(ptr2 != NULL, "Failed to alloc ptr");
	void *ptr = my_realloc(ptr2, 0);
	cr_expect(ptr == NULL, "Error realloc not freeing with size 0");
}*/

//Ajouter un test pour la demande d'une nouvelle meta page
