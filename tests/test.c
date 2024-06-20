#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void SetupPagesTest() {
	t_alloc_info *alloc_info = get_info();
	void *ptr = ft_malloc(12);
	write(2, "aaaa", 4);
	TEST_ASSERT_TRUE(alloc_info->tiny.chunks != NULL);

	void *ptr1 = ft_malloc(560);
//	TEST_ASSERT_TRUE(alloc_info->small.chunks != NULL);

	void *ptr2 = ft_malloc(4096);
//	TEST_ASSERT_TRUE(alloc_info->large.chunks != NULL);

	TEST_ASSERT_TRUE(ptr);
	TEST_ASSERT_TRUE(ptr1);
	TEST_ASSERT_TRUE(ptr2);
}
int main() {
   UNITY_BEGIN();

   RUN_TEST(SetupPagesTest);

   UNITY_END();
}
