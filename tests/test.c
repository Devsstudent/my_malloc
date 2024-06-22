#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void PagesTypeTest() {
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
}

int main() {
   UNITY_BEGIN();

   RUN_TEST(PagesTypeTest);

   UNITY_END();
}
