#include "malloc.h"
#include <stdlib.h>

//Global
t_alloc_info alloc_info;

//Initialiser la heap -> Setup les 3 pages / Set un block free de pages size
// -> Preallocate 

// ** Plus opti d'avoir deux list je pense **

//void *get_chunk(void *ptr, t_state) {

//}

//we can add the len of free and the len of busy to get the total len
//then if this size + the size of the new block is > nb_page * PAGE_SIZE
//we need to ask for a new page, we may have another address, in this case:
//
//we have to copy the whole memory of the page to the new one 
//that implies to change the address of each block of the list .. (Best would be to valeur absolue la difference) et de l'ajouter a tous les chunk addr


//pour le insert dans un chunks: should be fine


//

bool	init_page(size_t size, t_page page) {
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	return (true);
}


void *ft_malloc(size_t size) {
	if (!alloc_info.tiny->first_addr) {
		if (!init_page(page_size * 10, alloc_info.tiny)) {
			return (NULL);
		}
	}
	if (!alloc_info.small->first_addr) {
		if (!init_page(page_size * 5, alloc_info.small)) {
			return (NULL);
		}
	}
	if (!alloc_info.large->first_addr) {
		if (!init_page(page_size * 1, alloc_info.large)) {
			return (NULL);
		}
	}
	return (malloc(size));
}

void ft_free(void *ptr) {

}

void *ft_realloc(void *ptr, size_t size) {
	return (realloc(ptr, size));
}

//#Dynamic 
