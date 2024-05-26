#include "my_secmalloc.private.h"

int main() {
	void *a;

	a = my_malloc(12);
	my_free(a);
}
