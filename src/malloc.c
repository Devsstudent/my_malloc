#include "malloc.h"

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
	if (page->total_bytes_free < size || page->total_bytes_free - sizeof(t_chunk) <= 0) {
		return ;
	}
	if (!buff) {
		page->chunks = (t_chunk *) addr;
		page->chunks->size = size - sizeof(t_chunk);
		page->chunks->state = FREE;
		page->chunks->chunk_addr = addr;
		page->free_chunks += 1;
		page->last = page->chunks;
		return ;
	}
	while (buff->next) {
		buff = buff->next;
	}
	int	alignement_needed = (size_t)(buff->chunk_addr + buff->size) % 8;
	if (size + alignement_needed > page->total_bytes_free) {
		alignement_needed = ft_abs(8 - alignement_needed);
		if (size <= alignement_needed) {
			return ;
		}
		size -= alignement_needed;
	}
	page->total_bytes_free += size - sizeof(t_chunk);
	page->total_bytes_alignement += alignement_needed;
	buff->next = (t_chunk *) (buff->chunk_addr +  sizeof(t_chunk) + buff->size + alignement_needed);
	t_chunk *new = buff->next;
	new->size = size;
	new->state = FREE;
	new->chunk_addr = addr;
	page->free_chunks += 1;
	page->last = new;
}

bool	init_page(size_t size, t_pages *page) {
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	page->page_nb += 1;
	page->total_bytes_free = size;
	add_back(size, ptr, page);
	return (true);
}
#include <string.h>
bool	ask_new_page(size_t size, t_pages *page) {
	printf("SIZE %ld\n", size);
	void *ptr = mmap(page->chunks, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	if (ptr != page->chunks) {
		t_chunk *buff = page->chunks;
		ft_memmove(ptr, page->chunks, page->page_nb * PAGE_SIZE);
		printf("%p %p\n", ptr, page->chunks);
		page->chunks = (t_chunk *) ptr;
		printf("%p\n",page->chunks);
		if (munmap(buff, page->page_nb * PAGE_SIZE) < 0) {
			return (false);
		}
		printf("%p\n",page->chunks);
	}
	if (size % 4096 != 0) {
		page->page_nb = size / 4096 + 1;
	} else {
		page->page_nb = size / 4096;
	}
	t_chunk *buff = page->chunks;
	printf("%p %ld %ld\n", buff, buff->state, buff->size);
	while (buff && buff->next) {
		buff = buff->next;
	}
	page->last = buff;
	return (true);
}

bool setup_pages(t_pages *current) {
	if (!current->chunks && current->type == TINY) {
		if (!init_page(PAGE_SIZE * 10, current)) {
			return (false);
		}
	}
	if (!current->chunks && current->type == SMALL) {
		if (!init_page(PAGE_SIZE * 5, current)) {
			return (false);
		}
		alloc_info.tiny.page_nb += 1;
	}
	if (!current->chunks && current->type == LARGE) {
		if (!init_page(PAGE_SIZE * 1, current)) {
			return (false);
		}
	}
	return (true);
}

t_chunk *looking_for_chunk(t_pages *page, size_t size) {
	t_chunk *buff = page->chunks;

	while (buff) {
		//Because size contain sizeof(chunk)
		printf("state %i size %ld\n", buff->state, buff->size);
		if (buff->state == FREE && buff->size >= size) {
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

void *ft_malloc(size_t size) {
	void *alloc_addr = NULL;
	t_pages *current_pages = get_current_page(size + sizeof(t_chunk));
	if (!setup_pages(current_pages)) {
		return (false);
	}
	//checker la taille du block, et verifier qu'on au moins 1 block free qui correspond
	t_chunk *available_chunk = looking_for_chunk(current_pages, size);
	if (!available_chunk) {

		//new page mmap sur la page
		//ce qui implique le deplacement des donnee en memoire
		ask_new_page(PAGE_SIZE * current_pages->page_nb + size + sizeof(t_chunk), current_pages);
		if (current_pages->last->state == FREE) {
			printf("SIZE %ld %ld\n", current_pages->last->size, size);
			current_pages->last->size += size;
			current_pages->total_bytes_free += size;
		}
		available_chunk = current_pages->last;
		printf("SIZsssE %ld\n", available_chunk->size);
//	else  maybe create a block
	}
	available_chunk->state = BUSY;
	available_chunk->size = size;
	current_pages->busy_chunks += 1;
	current_pages->total_bytes_busy += size;
	alloc_addr = available_chunk->chunk_addr + sizeof(t_chunk);
	if (!available_chunk->next) {
		add_back(current_pages->total_bytes_free - size, available_chunk->chunk_addr + available_chunk->size, current_pages);
	}
	//split le chunk si le next n'est pas null
	//	return (current_pages->chunks->chunk_addr + sizeof(t_chunk));
	return (alloc_addr);
}

void ft_free(void *ptr) {

}

void *ft_realloc(void *ptr, size_t size) {
	return (realloc(ptr, size));
}

//#Dynamic 
