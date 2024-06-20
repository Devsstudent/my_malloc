#include "unity.h"
#include "malloc.h"

void setUp() {}
void tearDown() {}

void MallocTest() {
	void *ptr = ft_malloc(12);
	TEST_ASSERT_TRUE(ptr);
}
int main() {
   UNITY_BEGIN();

   RUN_TEST(MallocTest);

   UNITY_END();
}
