#include "include/malloc.h"
//#include <stdlib.h>

int main() {
   int   i;
   char  *addr;

   i = 0;
   while (i < 1024)
   {
      addr = (char*)ft_malloc(450);
      addr[0] = 42;
     ft_free(addr);
      i++;
   }
	i = 0;
	while (i < 120) {
      addr = (char*)ft_malloc(450);
		i++;
	}
   return (0);
}
