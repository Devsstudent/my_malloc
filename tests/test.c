/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 15:44:39 by odessein          #+#    #+#             */
/*   Updated: 2024/09/07 13:52:48 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void basic_one_allocation() {
	printf("Basic one allocation test\n");
	char *ptr = ft_malloc(151);
	TEST_ASSERT_TRUE(ptr != NULL);
	int i = 0;
	while (i < 150) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	printf("%s\n", ptr);
}
/*
void OneAllocationByPageTest() {
	t_alloc_info *alloc_info = get_info();

	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr != NULL);
	t_chunk *c = (t_chunk *)(ptr - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->tiny.chunks == c);
	TEST_ASSERT_TRUE(c->size == 12);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks->state == BUSY);

	ft_free(ptr);

	void *ptr1 = ft_malloc(560);
	TEST_ASSERT_TRUE(ptr1 != NULL);
	c = (t_chunk *)(ptr1 - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->small.chunks == c);
	TEST_ASSERT_TRUE(c->size == 560);
	TEST_ASSERT_TRUE(c->state == BUSY);

	alloc_info = get_info();
	printf("%p, %i %li \n", c, c->page_type, alloc_info->small.busy_chunks);
	ft_free(ptr1);
	TEST_ASSERT_TRUE(alloc_info->small.busy_chunks == 0);

	void *ptr2 = ft_malloc(4096);

	TEST_ASSERT_TRUE(ptr2 != NULL);
	c = (t_chunk *)(ptr2- sizeof(t_chunk));

	printf("%p %li \n", c, alloc_info->large.busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->large.chunks == c);
	TEST_ASSERT_TRUE(c->size == 4096);
	TEST_ASSERT_TRUE(c->state == BUSY);

	ft_free(ptr2);
	alloc_info = get_info();
	TEST_ASSERT_TRUE(alloc_info->large.busy_chunks == 0);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == 0);
	//show_alloc_mem();
}

void FreeTest() {
	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr != NULL);
	void *ptr2 = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr2 != NULL);


	ft_free(ptr);

	t_chunk *chunk = (t_chunk *) (ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(chunk->state == FREE);
	ft_free(ptr2);
	chunk = (t_chunk *) (ptr2 - sizeof(t_chunk));
	TEST_ASSERT_TRUE(chunk->state == FREE);
	//Maybe check qu'on unmap
}

void TwoAlloctionTiny() {
	t_alloc_info *alloc_info = get_info();
	//size_t	free_chunks = alloc_info->tiny.free_chunks;
	size_t	busy_chunks = alloc_info->tiny.busy_chunks;
	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr != NULL);
	busy_chunks += 1;
	if (free_chunks > 0) {
		free_chunks -= 1;
	} else {
		free_chunks += 1;
	}
	t_chunk *c = (t_chunk *)(ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks == c);
	TEST_ASSERT_TRUE(c->size == 12);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks->state == BUSY);


	void *ptr2 = ft_malloc(21);
	busy_chunks += 1;
	TEST_ASSERT_TRUE(ptr2 != NULL);
	c = get_chunk(ptr2, &alloc_info->tiny);
	TEST_ASSERT_TRUE(c->size == 21);
	TEST_ASSERT_TRUE(c->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == busy_chunks);
//	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_busy = 21);
//	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_free = alloc_info->tiny.page_nb * PAGE_SIZE - alloc_info->tiny.total_bytes_alignement - alloc_info->tiny.total_bytes_busy);

	ft_free(ptr);
	ft_free(ptr2);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == 0);
}

void MultipleAllocatoinTiny() {

	t_alloc_info *alloc_info = get_info();
	int rows = 1000;
	int cols = 500;

    // Dynamically allocate memory for a 2D array of characters
    char **matrix = (char **)ft_malloc(rows * sizeof(char *));
	TEST_ASSERT_TRUE(matrix != NULL);

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char *)ft_malloc((cols + 1) * sizeof(char)); // +1 for null terminator
		//printf("i %i %p\n", i, matrix[i]);

		TEST_ASSERT_TRUE(matrix[i] != NULL);
    }
    // Fill the 2D array with sample data
    char fillChar = 'A';
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
			if (i == 27) {
	//			show_alloc_mem();
			}
			printf("i %i j %i %p\n", i, j, matrix[i]);
            matrix[i][j] = fillChar++;
        }
        matrix[i][cols] = '\0'; // Null-terminate each string
    }
    // Free dynamically allocated memory
    for (int i = 0; i < rows; i++) {
        ft_free(matrix[i]);
    }
    ft_free(matrix);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == 0);
	//Loop allocation //check que c'est bien set
	//Loop free //check que c'est bien

	//tester les cas tricky avec des size bizarre (randomisation de la size allouer entre x et y)
}

void MultipleAllocationSmall() {
	int rows = 64;
	int cols = 1024;

    // Dynamically allocate memory for a 2D array of characters
	t_alloc_info *alloc_info = get_info();
	printf(" start %li\n", alloc_info->small.busy_chunks);
    char **matrix = (char **)ft_malloc(rows * sizeof(char *));
	TEST_ASSERT_TRUE(matrix != NULL);

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char *)ft_malloc((cols + 1) * sizeof(char)); // +1 for null terminator
		TEST_ASSERT_TRUE(matrix[i] != NULL);
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
        ft_free(matrix[i]);
    }
    ft_free(matrix);
	printf("%li\n", alloc_info->large.busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->small.busy_chunks == 0);
}

void MultipleAllocationLarge() {
	int rows = 64;
	int cols = 5600;

    // Dynamically allocate memory for a 2D array of characters
	t_alloc_info *alloc_info = get_info();
	printf(" start %li\n", alloc_info->large.busy_chunks);
    char **matrix = (char **)ft_malloc(rows * sizeof(char *));
	TEST_ASSERT_TRUE(matrix != NULL);

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char *)ft_malloc((cols + 1) * sizeof(char)); // +1 for null terminator
		TEST_ASSERT_TRUE(matrix[i] != NULL);
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
        ft_free(matrix[i]);
    }
    ft_free(matrix);
	printf("%li\n", alloc_info->large.busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->large.busy_chunks == 0);
}

void MultipleAllocationLoopMix() {
    t_alloc_info *alloc_info = get_info();

    // Array of mixed sizes
    size_t sizes[] = {8, 16, 32, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // Allocate a large number of blocks for each size, then free them
    for (size_t i = 0; i < num_sizes; i++) {
        size_t size = sizes[i];
        void *ptrs[100]; // Array to hold the allocated pointers
        size_t busy_chunks_before = 0;

        // Allocate blocks of memory for the current size
        for (int j = 0; j < 100; j++) {
            ptrs[j] = ft_malloc(size);
            TEST_ASSERT_TRUE(ptrs[j] != NULL);

            // Check allocation information
            t_chunk *c = (t_chunk *)(ptrs[j] - sizeof(t_chunk));
            TEST_ASSERT_TRUE(c->size == size);
            TEST_ASSERT_TRUE(c->state == BUSY);

            // Re-fetch alloc_info to check busy chunks
            alloc_info = get_info();
            if (size < 560) { // Assuming 512 bytes or less is tiny
                TEST_ASSERT_TRUE(alloc_info->tiny.chunks == c || alloc_info->tiny.chunks->state == BUSY);
                busy_chunks_before = alloc_info->tiny.busy_chunks;
            } else if (size < 4096) { // Assuming small is between 513 and 4096 bytes
                TEST_ASSERT_TRUE(alloc_info->small.chunks == c || alloc_info->small.chunks->state == BUSY);
                busy_chunks_before = alloc_info->small.busy_chunks;
            } else { // Larger than 4096 bytes is large
                TEST_ASSERT_TRUE(alloc_info->large.chunks == c || alloc_info->large.chunks->state == BUSY);
                busy_chunks_before = alloc_info->large.busy_chunks;
            }

            // Ensure busy chunks count increases
            TEST_ASSERT_TRUE(busy_chunks_before == (size_t)(j + 1));
        }

        // Free all allocated memory
        for (int j = 0; j < 100; j++) {
            ft_free(ptrs[j]);

            // Re-fetch alloc_info and check chunk state
            t_chunk *c = (t_chunk *)(ptrs[j] - sizeof(t_chunk));
            alloc_info = get_info();
            TEST_ASSERT_TRUE(c->state == FREE);

            // Ensure busy chunks count decreases
            size_t busy_chunks_after;
            if (size < 560) {
                busy_chunks_after = alloc_info->tiny.busy_chunks;
            } else if (size < 4096) {
                busy_chunks_after = alloc_info->small.busy_chunks;
            } else {
                busy_chunks_after = alloc_info->large.busy_chunks;
            }
            TEST_ASSERT_TRUE(busy_chunks_after == (size_t)(99 - j));
        }

        // Ensure all chunks are freed for this size
        alloc_info = get_info();
        if (size <= 512) {
            TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == 0);
        } else if (size <= 4096) {
            TEST_ASSERT_TRUE(alloc_info->small.busy_chunks == 0);
        } else {
            TEST_ASSERT_TRUE(alloc_info->large.busy_chunks == 0);
        }
    }
}
*/

//Pour le mix on va tester sur un char ** avec des strings bien fat puis des toute petite etc

int main() {
   UNITY_BEGIN();

   RUN_TEST(basic_one_allocation);

/*
   printf("\n\n FREE TEST \n\n");
   RUN_TEST(FreeTest);
   printf("\n\n ONE ALLOCATOIN BY PAGE TEST \n\n");
   RUN_TEST(OneAllocationByPageTest);

   printf("\n\n MULTIPLE ALLOCATION TINY TEST \n\n");
   RUN_TEST(TwoAlloctionTiny);

   printf("\n\n MULTIPLE ALLOCATION LOOP TINY \n\n");
   RUN_TEST(MultipleAllocatoinTiny);

   printf("\n\n MULTIPLE ALLOCATION LOOP SMALL \n\n");
   RUN_TEST(MultipleAllocationSmall);
   printf("\n\n MULTIPLE ALLOCATION LOOP LARGE \n\n");
   RUN_TEST(MultipleAllocationLarge);
   printf("\n\n MULTIPLE ALLOCATION LOOP MIX \n\n");
   RUN_TEST(MultipleAllocationLoopMix);
	show_alloc_mem();
	*/
   UNITY_END();
}
