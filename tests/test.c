/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 15:44:39 by odessein          #+#    #+#             */
/*   Updated: 2024/06/23 21:58:41 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void OneAllocationByPageTest() {
	t_alloc_info *alloc_info = get_info();

	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr != NULL);
	t_chunk *c = (t_chunk *)(ptr - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->tiny.chunks == c);
	TEST_ASSERT_TRUE(c->size == 12);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->tiny.page_nb == 10);

	ft_free(ptr);

	void *ptr1 = ft_malloc(560);
	TEST_ASSERT_TRUE(ptr1 != NULL);
	c = (t_chunk *)(ptr1 - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->small.chunks == c);
	TEST_ASSERT_TRUE(c->size == 560);
	TEST_ASSERT_TRUE(c->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->small.page_nb == 5);

	ft_free(ptr1);

	void *ptr2 = ft_malloc(4096);

	TEST_ASSERT_TRUE(ptr2 != NULL);
	c = (t_chunk *)(ptr2- sizeof(t_chunk));

	printf("%p %p \n", c, alloc_info->large.chunks);
	TEST_ASSERT_TRUE(alloc_info->large.chunks == c);
	TEST_ASSERT_TRUE(c->size == 4096);
	TEST_ASSERT_TRUE(c->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->large.page_nb == 3);

	ft_free(ptr2);
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
}

void TwoAlloctionTiny() {
	t_alloc_info *alloc_info = get_info();
	size_t	free_chunks = alloc_info->tiny.free_chunks;
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

	ft_free(ptr);

	void *ptr2 = ft_malloc(21);
	TEST_ASSERT_TRUE(ptr2 != NULL);
	c = get_chunk(ptr2, &alloc_info->tiny);
	TEST_ASSERT_TRUE(c->size == 21);
	TEST_ASSERT_TRUE(c->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_busy = 21);
	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_free = alloc_info->tiny.page_nb * PAGE_SIZE - alloc_info->tiny.total_bytes_alignement - alloc_info->tiny.total_bytes_busy);

	ft_free(ptr2);
}

void MultipleAllocatoinTiny() {

	int rows = 64;
	int cols = 500;

    // Dynamically allocate memory for a 2D array of characters
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
	//Loop allocation //check que c'est bien set
	//Loop free //check que c'est bien

	//tester les cas tricky avec des size bizarre (randomisation de la size allouer entre x et y)
}

void MultipleAllocationSmall() {
	int rows = 64;
	int cols = 1024;

    // Dynamically allocate memory for a 2D array of characters
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

}

void MultipleAllocationLarge() {
	int rows = 64;
	int cols = 5600;

    // Dynamically allocate memory for a 2D array of characters
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

}


//Pour le mix on va tester sur un char ** avec des strings bien fat puis des toute petite etc

int main() {
   UNITY_BEGIN();

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
   UNITY_END();
}
