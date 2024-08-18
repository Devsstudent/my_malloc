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
/*
size_t	get_current_free_space(t_pages *page) {
		printf("%ld %ld %ld\n", page->total_bytes_free, page->free_chunks, page->busy_chunks);
	return (page->total_bytes_free - (page->free_chunks + page->busy_chunks) * sizeof(t_chunk));
}*/

void	add_back(size_t size, void *addr, t_pages *page) {
	ft_printf("CALL ADDBACK\n");
	t_chunk *buff = page->chunks;
	//printf("size asked for the free block %ld %p\n", size, buff);
	if (!buff) {
		page->chunks = (t_chunk *) (addr + sizeof(t_chunk));
		page->chunks->size = size - sizeof(t_chunk);
		page->chunks->state = FREE;
		page->free_chunks += 1;
		page->last = page->chunks;
		page->total_bytes_free = size - sizeof(t_chunk);
		//printf("OUT ADDBACK %ld %ld \n", page->chunks->size, sizeof(t_chunk));
		return ;
	}
	size_t	free_space = page->total_bytes_free;
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
		if (size <= (size_t)alignement_needed) {
			return ;
		}
		size -= alignement_needed;
	}
	page->total_bytes_alignement += alignement_needed;
	page->total_bytes_free -= alignement_needed;
	buff->next = (t_chunk *) ((void *)buff +  sizeof(t_chunk) + buff->size + alignement_needed);
	t_chunk *new = buff->next;
	new->size = size - sizeof(t_chunk);
	new->next = NULL;
	new->alignement = alignement_needed;
	new->state = FREE;
	page->free_chunks += 1;
	page->last = new;
	//printf("OUT ADDBACK %p\n", new);
}

bool	init_page(size_t size, t_pages *page) {
	ft_printf("Init page type %i\n", page->type);
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	if (size % 4096 != 0) {
		page->page_nb = size / 4096 + 1;
	} else {
		page->page_nb = size / 4096;
	}
//	ft_printf("page nb %i type %i\n", page->page_nb, page->type);
	add_back(size, ptr, page);
	return (true);
}


//Idea functin that take a size and that transform it to the closeest PAGE_SIZE multiple

bool	ask_new_page(size_t size, t_pages *page) {
	//printf("SIZE NEW PAGE %ld\n", size);
	void *ptr = mmap(page->chunks, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		ft_printf("new page failed\n");
		return (false);
	}
	if (ptr != page->chunks) {
		ft_printf("new page diff\n");
		ft_printf("size %zu\n", size);
		page->last->next = (t_chunk *) ptr;
		page->last = page->last->next;
		page->last->size = size - sizeof(t_chunk);
		page->last->alignement = 0;
		page->last->state = FREE;
	} else {
		ft_printf("new page not failed, different\n");
	}
	//printf("%p %ld %ld\n", buff, buff->state, buff->size);
	if (size % 4096 != 0) {
		page->page_nb += size / 4096 + 1;
	} else {
		page->page_nb += size / 4096;
	}
	page->total_bytes_free += size - sizeof(t_chunk);
	if (page->last->state == FREE && ptr == page->chunks) {
		page->last->size += size;
	}
	page->last->next = NULL;
	show_alloc_mem();
	return (true);
}

bool setup_pages(t_pages *current, size_t size) {
//	printf("addr init first chunk %p \n", current->chunks);
	if (!current->chunks && current->type == TINY) {
		if (!init_page(PAGE_SIZE * 10, current)) {
			return (false);
		}
	}
	if (!current->chunks && current->type == SMALL) {
		if (!init_page(PAGE_SIZE * 12, current)) {
			return (false);
		}
	}
	if (!current->chunks && current->type == LARGE) {
		if (size > (size_t) (PAGE_SIZE * 100) && !init_page(size + sizeof(t_chunk), current)) {
			return (false);
		} else if (!init_page(PAGE_SIZE * 100, current)) {
			return (false);
		}
	}
	return (true);
}

t_chunk *looking_for_chunk(t_pages *page, size_t size) {
	t_chunk *buff = page->chunks;
	ft_printf("size requested %zu\n", size);
		while (buff) {
				//Because size contain sizeof(chunk)
		//printf("state %i size %ld asked size %ld addr %p\n", buff->state, buff->size, size, buff);
		if (buff->state == FREE && buff->size >= size) {
			ft_printf("size of the fineded block %zu\n", buff->size);
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

	//Check sur quelle page de base on est SMALL MEDIUM ETC
//	ft_putstr_fd("size ", STDERR_FILENO);
//	ft_putnbr_fd(size, STDERR_FILENO);
	t_pages *current_pages = get_current_page(size);
	if (!setup_pages(current_pages, size)) {
		return (false);
	}
	ft_printf("page type %i\n", current_pages->type);
	//checker la taille du block, et verifier qu'on au moins 1 block free qui correspond
	t_chunk *available_chunk = looking_for_chunk(current_pages, size);
	if (!available_chunk) {

		//new page mmap sur la page

		//Why assking that much ??
		ask_new_page(PAGE_SIZE * current_pages->page_nb + size + sizeof(t_chunk), current_pages);
		//Always last block free, otherwise must addback
		available_chunk = current_pages->last;
		ft_printf("size asdked%zu\n", size);
		ft_printf("%p %zu\n", available_chunk);
//	else  maybe create a block
	}
	size_t free_space_last_chunk = available_chunk->size;
	available_chunk->state = BUSY;
	available_chunk->size = size;
	current_pages->busy_chunks += 1;
	current_pages->free_chunks -= 1;
	//ft_printf("free before:%zu\n", current_pages->total_bytes_free);
	current_pages->total_bytes_busy += size;
	current_pages->total_bytes_free -= size;
	//ft_printf("free after:%zu\n", current_pages->total_bytes_free);
	alloc_addr = (void *) available_chunk + sizeof(t_chunk);
	//printf("total bytes free : %zu asked size %ld\n", current_pages->total_bytes_free, size);
	if (!available_chunk->next) {
//		if (current_pages->total_bytes_free < size) {
//			return (NULL);
//		}
		add_back(free_space_last_chunk - size + sizeof(t_chunk), (void *)alloc_addr + available_chunk->size, current_pages);
	} else {
		//insert entre le chunk et le suivant le reste du chunk
		ft_printf("il y a un next\n");
	}
	//printf("%p\n", alloc_addr);k
	//split le chunk si le next n'est pas null
	//return (current_pages->chunks->chunk_addr + sizeof(t_chunk));
	return (alloc_addr);
}

t_pages	*get_ptr_page(void *ptr) {

	//printf("wtf %p\n", ptr);
	if (alloc_info.tiny.busy_chunks != 0 && (size_t) ptr >= (size_t)(alloc_info.tiny.chunks) && (size_t) ptr <= (size_t)alloc_info.tiny.last) {
		return (&alloc_info.tiny);
	}
	if (alloc_info.small.busy_chunks != 0 && alloc_info.small.chunks && (size_t) ptr >= (size_t)(alloc_info.small.chunks) && (size_t) ptr <= (size_t)alloc_info.small.last) {
		return (&alloc_info.small);
	}
	if (alloc_info.large.busy_chunks != 0 && alloc_info.large.chunks && (size_t) ptr >= (size_t)(alloc_info.large.chunks) && (size_t) ptr <= (size_t)alloc_info.large.last) {
		return (&alloc_info.large);
	}
	return (NULL);
}

t_chunk *get_chunk(void *ptr, t_pages *ptr_page) {
	t_chunk *buff = ptr_page->chunks;
	while (buff) {
		if ((void *)buff + sizeof(t_chunk) == ptr) {
			return (ptr - sizeof(t_chunk));
		}
		buff = buff->next;
	}
	return (NULL);
}


//Pour le free : Checker dans get_chunk lentre deux chunk (pour free meme au milieux d'un chunk

void ft_free(void *ptr) {
	if (!ptr) {
		return;
	}
	t_pages *ptr_page = get_ptr_page(ptr);
	if (!ptr_page) {
		return ;
	}
	//printf("TYPE PAGE TO FREE : %i %zu\n", ptr_page->type, ptr_page->busy_chunks);
	t_chunk *chunk_to_free = get_chunk(ptr, ptr_page);
	//printf("test %p %ld\n", chunk_to_free, chunk_to_free->size);
	if (!chunk_to_free) {
		return ;
	}
	//fonction de changement d'etat ? 
	chunk_to_free->state = FREE;
	ptr_page->total_bytes_free += chunk_to_free->size;
	ptr_page->total_bytes_busy -= chunk_to_free->size;
	ptr_page->free_chunks += 1;
	ptr_page->busy_chunks -= 1;
	//printf("GROUUU %ld %ld %ld\n", ptr_page->total_bytes_free, chunk_to_free->size,ptr_page->busy_chunks);
	if (ptr_page->busy_chunks == 0) {
		munmap(ptr_page->chunks, ptr_page->page_nb * PAGE_SIZE);
		ft_memset(ptr_page, 0, sizeof(t_pages));
	}
}


//Pour le realloc, comme pour le free, si address au milieux augmenter la size etc
void *ft_realloc(void *ptr, size_t size) {
	//To Do
	//Look to change size of ptr to size
	//If it is smaller than acutal, we just add a free chunk in between ptr and ptr->next
	//else look after if there is a free chunk
	//else call malloc and return the new address setting the previous to free
	//copy content to the new address if it changes
	ft_printf("%p %zu call realooc\n", ptr, size);
	void *res = NULL;
	if (size == 0 && ptr) {
		ft_free(ptr);
		return (res);
	}
	if (!ptr && size > 0) {
		res = ft_malloc(size);
		if (res) {
			return (res);
		}
	}
	t_pages *ptr_page = get_ptr_page(ptr);
	if (!ptr_page) {
		return (res);
	}
	t_chunk *info_ptr = get_chunk(ptr, ptr_page);
	if (info_ptr) {
		res = ft_malloc(size);
		ft_memcpy(res, ptr, info_ptr->size);
		ft_free(ptr);
		return (res);
	}
	return res;
}

size_t	browse_chunks(t_pages *pages) {
	t_chunk	*chunk = pages->chunks;
	if (!chunk) {
		return 0;
	}
	t_chunk *next = chunk->next;
	size_t	alloced_bytes = 0;
	size_t	free_bytes = 0;
	while (chunk) {
		next = chunk->next;
		if (chunk->state == BUSY) {
			ft_printf("%p - %p: %zu bytes\n", chunk, next, chunk->size);
			alloced_bytes += chunk->size;
		} else {
			ft_printf("FREE ? %p - %p: %zu bytes\n", chunk, next, chunk->size);
			free_bytes += chunk->size;
		}
		chunk = next;
	}
	ft_printf("free total = %zu in memory %zu\n", free_bytes, pages->total_bytes_free);
	return alloced_bytes;
}

void	show_alloc_mem() {
	size_t	alloced_bytes = 0;
	ft_printf("%s %p\n", "TINY:", alloc_info.tiny.chunks);
	alloced_bytes += browse_chunks(&alloc_info.tiny);
	ft_printf("%s %p\n", "SMALL:", alloc_info.small.chunks);
	alloced_bytes += browse_chunks(&alloc_info.small);
	ft_printf("%s %p\n", "LARGE:", alloc_info.large.chunks);
	alloced_bytes += browse_chunks(&alloc_info.large);
	ft_printf("Total alloc: %zu bytes\n", alloced_bytes);
}

#ifdef DYNAMIC

//#warning "DYNAMIC is defined"
void    *malloc(size_t size)
{
//    ft_putstr_fd("malloc\n", STDOUT_FILENO);
    void *ptr = ft_malloc(size);
	//show_alloc_mem();
	ft_printf("%p returned\n", ptr);
	return (ptr);
}
void    free(void *ptr)
{
    ft_putstr_fd("free\n", STDOUT_FILENO);
    ft_free(ptr);
}

void	*calloc(size_t nmemb, size_t size) {
	void *ptr = ft_malloc(size * nmemb);
	if (!ptr) {
		return (NULL);
	}
	ft_memset(ptr, 0, nmemb * size);
	return (ptr);
}

void    *realloc(void *ptr, size_t size)
{
	void *ptr2 = ft_realloc(ptr, size);
    ptr2 = ft_realloc(ptr, size);
	show_alloc_mem();
	return (ptr2);

}

#endif
