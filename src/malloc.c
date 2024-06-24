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
size_t	get_current_free_space(t_pages *page) {
		printf("%ld %ld %ld\n", page->total_bytes_free, page->free_chunks, page->busy_chunks);
	return (page->total_bytes_free - (page->free_chunks + page->busy_chunks) * sizeof(t_chunk));
}

void	add_back(size_t size, void *addr, t_pages *page) {
	printf("CALL ADDBACK\n");
	t_chunk *buff = page->chunks;
	printf("size asked for the free block %ld %p\n", size, buff);
	if (!buff) {
		page->chunks = (t_chunk *) addr;
		page->chunks->size = size - sizeof(t_chunk);
		page->chunks->state = FREE;
		page->free_chunks += 1;
		page->last = page->chunks;
		page->total_bytes_free = size;
		printf("OUT ADDBACK %ld %ld %ld \n", page->chunks->size, get_current_free_space(page), sizeof(t_chunk));
		return ;
	}
	size_t	free_space = get_current_free_space(page);
	/*
	if (page->total_bytes_free < size || page->total_bytes_free - sizeof(t_chunk) <= 0) {
		return ;
	}*/
	while (buff->next) {
		buff = buff->next;
	}
	int	alignement_needed = (size_t)((void *)buff + buff->size) % 8;
	if (size + alignement_needed > free_space) {
		alignement_needed = ft_abs(8 - alignement_needed);
		if (size <= alignement_needed) {
			return ;
		}
		size -= alignement_needed;
	}
	page->total_bytes_alignement += alignement_needed;
	printf("A HF %ld %p %ld\n", (size_t)(sizeof(t_chunk) + buff->size + alignement_needed), page->chunks, free_space);
	buff->next = (t_chunk *) ((void *)buff +  sizeof(t_chunk) + buff->size + alignement_needed);
	t_chunk *new = buff->next;
	new->size = size - sizeof(t_chunk);
	new->state = FREE;
	page->free_chunks += 1;
	page->last = new;
	printf("OUT ADDBACK\n");
}

bool	init_page(size_t size, t_pages *page) {
	printf("INIT PAGE\n");
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	page->page_nb += 1;
	add_back(size, ptr, page);
	return (true);
}

bool	ask_new_page(size_t size, t_pages *page) {
	printf("SIZE NEW PAGE %ld\n", size);
	void *ptr = mmap(page->chunks, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	t_chunk *buff = NULL;
	if (!ptr) {
		return (false);
	}
	if (ptr != page->chunks) {
		t_chunk *previous_location = page->chunks;
		ft_memmove(ptr, page->chunks, page->page_nb * PAGE_SIZE);
		printf("new addr %p %p bru %ld\n", ptr, page->chunks, page->page_nb * PAGE_SIZE);
		page->chunks = (t_chunk *) ptr;
		buff = page->chunks;
		printf("%p\n",page->chunks);
		if (munmap(previous_location, page->page_nb * PAGE_SIZE) < 0) {
			return (false);
		}
		printf("%p\n",page->chunks);
	}
	if (size % 4096 != 0) {
		page->page_nb = size / 4096 + 1;
	} else {
		page->page_nb = size / 4096;
	}
	//printf("%p %ld %ld\n", buff, buff->state, buff->size);
	while (buff && buff->next) {
		printf("testing\n");
		buff = buff->next;
	}
	page->last = buff;
	page->total_bytes_free += size;
	return (true);
}

bool setup_pages(t_pages *current) {
	printf("addr init first chunk %p \n", current->chunks);
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

	printf("\n\n");
	while (buff) {
		//Because size contain sizeof(chunk)
		printf("state %i size %ld asked size %ld aligned %ld\n", buff->state, buff->size, size, page->total_bytes_alignement);
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
		//Always last block free, otherwise must addback
		if (current_pages->last->state == FREE) {
			current_pages->last->size += size;
		}
		available_chunk = current_pages->last;
//	else  maybe create a block
	}
	available_chunk->state = BUSY;
	available_chunk->size = size;
	current_pages->busy_chunks += 1;
	current_pages->free_chunks -= 1;
	current_pages->total_bytes_busy += size;
	current_pages->total_bytes_free -= size;
	alloc_addr = (void *) available_chunk + sizeof(t_chunk);
	printf("total bytes free : %i \n", get_current_free_space(current_pages));
	if (!available_chunk->next) {
		if (current_pages->total_bytes_free < size) {
			return (NULL);
		}
		add_back(current_pages->total_bytes_free - size, (void *)available_chunk + available_chunk->size, current_pages);
	}
	//split le chunk si le next n'est pas null
	//return (current_pages->chunks->chunk_addr + sizeof(t_chunk));
	return (alloc_addr);
}

t_pages	*get_ptr_page(void *ptr) {

	if ((size_t) ptr >= (size_t)(alloc_info.tiny.chunks) && (size_t) ptr <= (size_t)alloc_info.tiny.last) {
		return (&alloc_info.tiny);
	}
	if ((size_t) ptr >= (size_t)(alloc_info.small.chunks) && (size_t) ptr <= (size_t)alloc_info.small.last) {
		return (&alloc_info.small);
	}
	if ((size_t) ptr >= (size_t)(alloc_info.large.chunks) && (size_t) ptr <= (size_t)alloc_info.large.last) {
		return (&alloc_info.large);
	}
	return (NULL);
}

t_chunk *get_chunk(void *ptr, t_pages *ptr_page) {
	t_chunk *buff = ptr_page->chunks;
	while (buff) {
		if ((void *)buff + sizeof(t_chunk) == ptr) {
			return (buff);
		}
		buff = buff->next;
	}
	return (NULL);
}

void ft_free(void *ptr) {
	if (!ptr) {
		return;
	}
	t_pages *ptr_page = get_ptr_page(ptr);
	if (!ptr_page) {
		return ;
	}
	t_chunk *chunk_to_free = get_chunk(ptr, ptr_page);
	if (!chunk_to_free) {
		return ;
	}
	//fonction de changement d'etat ? 
	chunk_to_free->state = FREE;
	ptr_page->total_bytes_free += chunk_to_free->size;
	ptr_page->total_bytes_busy -= chunk_to_free->size;
	printf("GROUUU %ld %ld\n", get_current_free_space(ptr_page), chunk_to_free->size);
	ptr_page->free_chunks += 1;
	ptr_page->busy_chunks -= 1;
}

void *ft_realloc(void *ptr, size_t size) {
	return (realloc(ptr, size));
}

//#Dynamic 
