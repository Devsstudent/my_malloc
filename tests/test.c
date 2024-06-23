/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 15:44:39 by odessein          #+#    #+#             */
/*   Updated: 2024/06/23 16:15:47 by odessein         ###   ########.fr       */
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


	void *ptr1 = ft_malloc(560);
	TEST_ASSERT_TRUE(ptr1 != NULL);
	c = (t_chunk *)(ptr1 - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->small.chunks == c);
	TEST_ASSERT_TRUE(c->size == 560);
	TEST_ASSERT_TRUE(c->state == BUSY);

	void *ptr2 = ft_malloc(4096);

	TEST_ASSERT_TRUE(ptr2 != NULL);
	c = (t_chunk *)(ptr2 - sizeof(t_chunk));

	TEST_ASSERT_TRUE(alloc_info->large.chunks == c);
	TEST_ASSERT_TRUE(c->size == 4096);
	TEST_ASSERT_TRUE(c->state == BUSY);

	ft_free(ptr);
	c = (t_chunk *) (ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(c->state == FREE);
	ft_free(ptr1);
	c = (t_chunk *) (ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(c->state == FREE);
	ft_free(ptr2);
	c = (t_chunk *) (ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(c->state == FREE);
}

void FreeTest() {
	void *ptr = ft_malloc(12);

	TEST_ASSERT_TRUE(ptr != NULL);

	ft_free(ptr);
	t_chunk *chunk = (t_chunk *) (ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(chunk->state == FREE);
}

void TwoAlloctionTiny() {
	t_alloc_info *alloc_info = get_info();
	size_t	free_chunks = alloc_info->tiny.free_chunks;
	size_t	busy_chunks = alloc_info->tiny.busy_chunks;
	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr != NULL);
	busy_chunks += 1;
	free_chunks -= 1;
	t_chunk *c = (t_chunk *)(ptr - sizeof(t_chunk));
	TEST_ASSERT_TRUE(alloc_info->tiny.free_chunks = free_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks == c);
	TEST_ASSERT_TRUE(c->size == 12);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks->state == BUSY);
	void *ptr2 = ft_malloc(21);
	TEST_ASSERT_TRUE(ptr2 != NULL);
	if (free_chunks > 1) {
		free_chunks -= 1;
	}
	busy_chunks += 1;
	c = get_chunk(ptr2, &alloc_info->tiny);
	TEST_ASSERT_TRUE(c->size == 21);
	TEST_ASSERT_TRUE(c->state == BUSY);
	TEST_ASSERT_TRUE(alloc_info->tiny.free_chunks == free_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.busy_chunks == busy_chunks);
	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_busy = 12 + 21);
	TEST_ASSERT_TRUE(alloc_info->tiny.total_bytes_free = alloc_info->tiny.page_nb * PAGE_SIZE - alloc_info->tiny.total_bytes_alignement - alloc_info->tiny.total_bytes_busy);
	ft_free(ptr);
	ft_free(ptr2);
}

void MultipleAllocatoinTiny() {
	//Loop allocation //check que c'est bien set
	//Loop free //check que c'est bien

	//tester les cas tricky avec des size bizarre (randomisation de la size allouer entre x et y)
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
   printf("\n\n MULTIPLE ALLOCATION LOOP SMALL \n\n");
   printf("\n\n MULTIPLE ALLOCATION LOOP LARGE \n\n");
   printf("\n\n MULTIPLE ALLOCATION LOOP MIX \n\n");
   UNITY_END();
}
