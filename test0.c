#include "include/ft_malloc.h"
//#include <stdlib.h>

int main() {
   int   i;
   char  *addr;

   i = 0;
   addr = (char*)ft_malloc(1);

   ft_free(addr);

	show_alloc_mem();
   return (0);
}
