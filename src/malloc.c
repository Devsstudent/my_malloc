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


//Utils.c
//
void	add_back(size_t size, void *addr, t_pages *page) {
	t_chunk *buff = page->chunks;
	page->free_chunks += 1;
	total_bytes_free += size - sizeof(t_chunk);
	if (!buff) {
		page->chunks = (t_chunk *) addr;
		page->chunks->size = size + sizeof(t_chunk);
		page->chunks->state = FREE;
		page->chunks->chunk_addr = addr;
		return ;
	}
	while (buff->next) {
		buff = buff->next;
	}
	buff->next = (t_chunk *) buff->chunk_addr + size;
	t_chunk *new = buff->next;
	new->size = size + sizeof(t_chunk);
	new->state = FREE;
	new->chunk_addr = addr;
}

bool	init_page(size_t size, t_page *page) {
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	page->page_nb += 1;
	add_back(size, ptr, page->chunks);
	return (true);
}

bool setup_pages(t_pages *current) {
	if (!current->chunks && current->type == TINY) {
		if (!init_page(page_size * 10, alloc_info.tiny)) {
			return (false);
		}
	}
	if (!current->chunks && current->type == SMALL) {
		if (!init_page(page_size * 5, alloc_info.small)) {
			return (false);
		}
		alloc_info.tiny.page_nb += 1;
	}
	if (!current->chunks && current->type == LARGE) {
		if (!init_page(page_size * 1, alloc_info.large)) {
			return (false);
		}
	}
	return (true);
}

t_chunk *looking_for_chunk(t_pages *page, size_t size) {
	t_chunk *buff = page->chunks;
	while (buff) {
		//Because size contain sizeof(chunk)
		if (buff->state == FREE && buff->size <= size) {
			return (buff);
		}
		buff = buff->next;
	}
	return NULL;
}


t_pages	*get_current_page(size_t size) {
	if (size >= 4096) {
		alloc_info.large.type = LARGE;
		return (&alloc_info.large);
	} else if (size >= 560) {
		alloc_info.small.type = SMALL;
		return (&alloc_info.small);
	}
	alloc_info.tiny.type = TINY;
	return (&alloc_info.tiny);
}

t_alloc_info *get_info(void){
	return &alloc_info;
}


#include <stdio.h>
void *ft_malloc(size_t size) {
	t_pages *current_pages = get_current_page(size);
	write(2, "AA", 2);
	if (!setup_pages()) {
		return (false);
	}
	//checker la taille du block, et verifier qu'on au moins 1 block free qui correspond
	
	if (!looking_for_chunk(current_pages, size)) {
		//new page mmap sur la page
		//ce qui implique le deplacement des donnee en memoire
	}
	//split le chunk si le next n'est pas null
	return (current_pages(chunks->chunk_addr + sizeof(chunk)));
}

void ft_free(void *ptr) {

}

void *ft_realloc(void *ptr, size_t size) {
	return (realloc(ptr, size));
}

//#Dynamic 
