/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 15:44:39 by odessein          #+#    #+#             */
/*   Updated: 2024/09/08 20:07:57 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void test_small_alloc_2_mem_zone()
{
	int rows = 101;
	int cols = 4050;

    // Dynamically allocate memory for a 2D array of characters
	//t_alloc_info *alloc_info = get_info();
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
	t_alloc_info info = get_alloc_info();
	TEST_ASSERT_TRUE(info.nb_small_elems == 2);
    // Free dynamically allocated memory
    //for (int i = 0; i < rows; i++) {
     //   ft_free(matrix[i]);
  //  }
   // ft_free(matrix);
	//printf("%li\n", alloc_info->large.busy_chunks);
	//TEST_ASSERT_TRUE(alloc_info->small.busy_chunks == 0);
}


void test_tiny_alloc_2_mem_zone()
{
	int rows = 104;
	int cols = 530;

    // Dynamically allocate memory for a 2D array of characters
	//t_alloc_info *alloc_info = get_info();
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
	t_alloc_info info = get_alloc_info();
	TEST_ASSERT_TRUE(info.nb_tiny_elems == 2);
    // Free dynamically allocated memory
    //for (int i = 0; i < rows; i++) {
     //   ft_free(matrix[i]);
  //  }
   // ft_free(matrix);
	//printf("%li\n", alloc_info->large.busy_chunks);
	//TEST_ASSERT_TRUE(alloc_info->small.busy_chunks == 0);
}

void basic_one_tiny_allocation() {
	printf("Basic one tiny allocation test\n");
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

void basic_one_small_allocation() {
	printf("Basic one small allocation test\n");
	char *ptr = ft_malloc(4000);
	TEST_ASSERT_TRUE(ptr != NULL);
	int i = 0;
	while (i < 3999) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	printf("%s\n", ptr);
}

void basic_one_large_allocation() {
	printf("Basic one large allocation test\n");
	char *ptr = ft_malloc(8192);
	TEST_ASSERT_TRUE(ptr != NULL);
	int i = 0;
	while (i < 8192) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	printf("%s\n", ptr);
}

char *basic_large_alloc() {
	char *ptr = ft_malloc(8192);
	if (!ptr) {
		return (NULL);
	}
	int i = 0;
	while (i < 8192) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	return (ptr);
}

char *basic_tiny_alloc() {
	char *ptr = ft_malloc(500 + 1);
	if (!ptr) {
		return (NULL);
	}
	int i = 0;
	while (i < 500) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	return (ptr);
}

char *basic_small_alloc() {
	char *ptr = ft_malloc(4000 + 1);
	if (!ptr) {
		return (NULL);
	}
	int i = 0;
	while (i < 4000) {
		ptr[i] = 'a';
		i++;
	}
	ptr[i] = 0;
	return (ptr);
}

void multi_large_allocation() {
	printf("Multi large allocation Test \n");
	char *ptr1 = basic_large_alloc();
	TEST_ASSERT_TRUE(ptr1 != NULL);
	char *ptr2 = basic_large_alloc();
	TEST_ASSERT_TRUE(ptr2 != NULL);
	char *ptr3 = basic_large_alloc();
	TEST_ASSERT_TRUE(ptr3 != NULL);
	char *ptr4 = basic_large_alloc();
	TEST_ASSERT_TRUE(ptr4 != NULL);

	t_alloc_info info = get_alloc_info();
	TEST_ASSERT_TRUE(info.nb_large_elems == 4);
	printf("%s %s %s %s\n", ptr1, ptr2, ptr3, ptr4);
}

void multi_tiny_allocation() {
	printf("Multi tiny allocation Test\n");
	char *ptr1 = basic_tiny_alloc();
	TEST_ASSERT_TRUE(ptr1 != NULL);
	char *ptr2 = basic_tiny_alloc();
	TEST_ASSERT_TRUE(ptr2 != NULL);
	char *ptr3 = basic_tiny_alloc();
	TEST_ASSERT_TRUE(ptr3 != NULL);
	char *ptr4 = basic_tiny_alloc();
	TEST_ASSERT_TRUE(ptr4 != NULL);
	printf("%s %s %s %s\n", ptr1, ptr2, ptr3, ptr4);
}

void multi_small_allocation() {
	printf("Multi small allocation Test\n");
	char *ptr1 = basic_small_alloc();
	TEST_ASSERT_TRUE(ptr1 != NULL);
	char *ptr2 = basic_small_alloc();
	TEST_ASSERT_TRUE(ptr2 != NULL);
	char *ptr3 = basic_small_alloc();
	TEST_ASSERT_TRUE(ptr3 != NULL);
	char *ptr4 = basic_small_alloc();
	TEST_ASSERT_TRUE(ptr4 != NULL);
	printf("%s %s %s %s\n", ptr1, ptr2, ptr3, ptr4);
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

   RUN_TEST(multi_large_allocation);

   RUN_TEST(test_small_alloc_2_mem_zone);

   RUN_TEST(test_tiny_alloc_2_mem_zone);

   RUN_TEST(basic_one_tiny_allocation);

   RUN_TEST(basic_one_small_allocation);

   RUN_TEST(basic_one_large_allocation);


   RUN_TEST(multi_tiny_allocation);
   RUN_TEST(multi_small_allocation);
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
