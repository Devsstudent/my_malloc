#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>
Test(mmap, simple) {
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_expect(ptr != NULL);
    int res = munmap(ptr, 4096);
    cr_expect(res == 0);
}

Test(my_malloc_simple, simple) {
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
}
Test(my_malloc_aligned, double_alloc_aligned_one_page) {
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(25);
	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + 1, "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12);
	void *ptr3 = my_malloc(2500);
	cr_expect((size_t) ptr3 == (size_t) ptr + 12 + 25 + 2, "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 25);
	void *ptr4 = my_malloc(1);
	cr_expect((size_t) ptr4 == (size_t) ptr + 12 + 25 + 2500 + 3, "Not aligned: %lx -%lx\n", (size_t)ptr4, (size_t)ptr + 12 + 25 + 2500);
}
Test(my_malloc_more_pages, more_pages){
	void *ptr = my_malloc(12);
	cr_expect(ptr != NULL, "Failed to alloc");
	void *ptr1 = my_malloc(200);
	cr_expect(ptr1 != NULL, "Failed to alloc");
	void *ptr2 = my_malloc(4000);
	cr_expect(ptr2 != NULL, "Failed to alloc");
	cr_expect((size_t) ptr2 == (size_t) ptr + 12 + 200 + 2, "Not aligned: %lx -%lx\n", (size_t)ptr2, (size_t)ptr + 12 + 200);
	void *ptr3 = my_malloc(400);
	cr_expect(ptr3 != NULL, "Failed to alloc ptr3");
	cr_expect((size_t) ptr3 == (size_t) ptr + 12 + 200 + 4000 + 3, "Not aligned: %lx -%lx\n", (size_t)ptr3, (size_t)ptr + 12 + 200 + 4000);
	void *ptr4 = my_malloc(9000);
	cr_expect(ptr4 != NULL, "Failed to alloc ptr4");
	cr_expect((size_t) ptr4 == (size_t) ptr + 12 + 200 + 4000 + 400 + 4, "Not aligned: %lx -%lx\n", (size_t)ptr4, (size_t)ptr + 12 + 200 + 4000 + 400);
}
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

//Test(my_free, free_first_chunk)
