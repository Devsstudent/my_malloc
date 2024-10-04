/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 18:33:21 by odessein          #+#    #+#             */
/*   Updated: 2024/08/19 19:06:13 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
//Global
//t_alloc_info alloc_info;

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
		//ft_printf("%ld %ld %ld\n", page->total_bytes_free, page->free_chunks, page->busy_chunks);
	return (page->total_bytes_free - (page->free_chunks + page->busy_chunks) * sizeof(t_chunk));
}*/
/*
void	add_back(size_t size, void *addr, t_pages *page) {
	//ft_printf("CALL ADDBACK\n");
	t_chunk *buff = page->chunks;
	////ft_printf("size asked for the free block %ld %p\n", size, buff);
	if (!buff) {
		page->chunks = (t_chunk *) (addr + sizeof(t_chunk));
		page->chunks->size = size - sizeof(t_chunk);
		page->chunks->state = FREE;
		page->chunks->page_type = page->type;
		page->free_chunks += 1;
		page->last = page->chunks;
	//	page->total_bytes_free += size - sizeof(t_chunk);
		////ft_printf("OUT ADDBACK %ld %ld \n", page->chunks->size, sizeof(t_chunk));
		return ;
	}
//	size_t	free_space = page->total_bytes_free;
	if (page->total_bytes_free < size || page->total_bytes_free - sizeof(t_chunk) <= 0) {
		return ;
	}
	t_chunk *last = page->last;
	int	alignement_needed = (size_t)((void *)buff + buff->size) % 8;
//	if (size + alignement_needed > free_space) {
//		alignement_needed = ft_abs(8 - alignement_needed);
//		if (size <= (size_t)alignement_needed) {
//			return ;
//		}
//		size -= alignement_needed;
//	}
	page->total_bytes_alignement += alignement_needed;
	//page->total_bytes_free -= alignement_needed;
	last->next = (t_chunk *) ((void *)last +  sizeof(t_chunk) + last->size + alignement_needed);
	t_chunk *new = last->next;
	//ft_printf("siiize %i, ali %i\n", size, alignement_needed);
	new->size = size;
	new->next = NULL;
	new->alignement = alignement_needed;
	new->state = FREE;
	new->page_type = page->type;
	page->free_chunks += 1;
	page->last = new;
	//page->total_bytes_free += new->size;
	////ft_printf("OUT ADDBACK %p\n", new);
}

bool	init_page(size_t size, t_pages *page) {
	//ft_printf("Init page type %i\n", page->type);
	t_type type = page->type;
	void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		return (false);
	}
	ft_memset(page, 0, sizeof(t_pages));
	if (size % 4096 != 0) {
		page->page_nb = size / 4096 + 1;
	} else {
		page->page_nb = size / 4096;
	}
	page->type = type;
//	//ft_printf("page nb %i type %i\n", page->page_nb, page->type);
	add_back(size, ptr, page);
	return (true);
}


//Idea functin that take a size and that transform it to the closeest PAGE_SIZE multiple

bool	ask_new_page(size_t size, t_pages *page) {
	////ft_printf("SIZE NEW PAGE %ld\n", size);
	size_t	real_mmap_allocated_size = size % 4096 != 0 ? size / 4096 + 1 : size / 4096;
	void *ptr = mmap(page->chunks, real_mmap_allocated_size * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (!ptr) {
		//ft_printf("new page failed\n");
		return (false);
	}
	real_mmap_allocated_size = real_mmap_allocated_size * PAGE_SIZE;

	ft_printf("size asked burh %i \n", real_mmap_allocated_size);

	if (ptr != page->chunks) {
		ft_printf("new page diff %i\n", page->type);
		//ft_printf("size %zu\n", real_mmap_allocated_size);
		ft_printf("%p\n", ptr);
		t_chunk *bb = page->last;
		page->last->next = (t_chunk *) ptr;
		page->last = page->last->next;
		page->last->size = real_mmap_allocated_size - sizeof(t_chunk);
		page->last->alignement = 0;
		page->last->page_type = page->type;
		page->last->state = FREE;
		page->free_chunks += 1;
		//ft_printf("%p %p %pnext\n", page->last, bb, bb->next);
		//page->total_bytes_free += page->last->size;
	} else {
		ft_printf("new page not failed same add\n");
	}
	////ft_printf("%p %ld %ld\n", buff, buff->state, buff->size);
	if (size % 4096 != 0) {
		page->page_nb += size / 4096 + 1;
	} else {
		page->page_nb += size / 4096;
	}
//	if (page->last->state == FREE && ptr == page->chunks) {
//		page->last->size += size;
//	}
	page->last->next = NULL;
	//show_alloc_mem();
	return (true);
}

bool setup_pages(t_pages *current, size_t size) {
//	//ft_printf("addr init first chunk %p \n", current->chunks);
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
	//ft_printf("size requested %zu\n", size);
		while (buff) {
				//Because size contain sizeof(chunk)
		////ft_printf("state %i size %ld asked size %ld addr %p\n", buff->state, buff->size, size, buff);
		//ft_printf("%p\n", buff);
		if (buff->state == FREE && buff->size >= size) {
			printf("size of the fineded block %zu\n", buff->size);
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

	t_pages *current_pages = get_current_page(size);
	if (!setup_pages(current_pages, size)) {
		return (false);
	}

	//checker la taille du block, et verifier qu'on au moins 1 block free qui correspond
	t_chunk *available_chunk = looking_for_chunk(current_pages, size);
	if (!available_chunk) {
		//new page mmap sur la page

		//Why assking that much ??
		//ask_new_page(PAGE_SIZE * current_pages->page_nb + size + sizeof(t_chunk), current_pages);
		ask_new_page(size + sizeof(t_chunk), current_pages);
		//Always last block free, otherwise must addback
		available_chunk = current_pages->last;
		//ft_printf("size asdked%zu\n", size);
		//ft_printf("%p %zu %i\n", available_chunk, size, available_chunk->state);
		//	else  maybe create a block
	}
	size_t free_space_last_chunk = available_chunk->size;
	available_chunk->state = BUSY;
	available_chunk->size = size;
	current_pages->busy_chunks += 1;
	current_pages->free_chunks -= 1;
//	//ft_printf("free before:%zu size %zu\n", current_pages->total_bytes_free, size);
	current_pages->total_bytes_busy += size;
//	current_pages->total_bytes_free -= size;
//	//ft_printf("free after:%zu size %zu\n", current_pages->total_bytes_free, size);
	alloc_addr = (void *) available_chunk + sizeof(t_chunk);
	//ft_printf("ci %p %lu\n", alloc_addr, sizeof(t_chunk));
	////ft_printf("total bytes free : %zu asked size %ld\n", current_pages->total_bytes_free, size);
	int free_space_left_in_chunk = (int)free_space_last_chunk - (int)((sizeof(t_chunk) + size));
	if (free_space_left_in_chunk <= 0) {
		//ft_printf("WTF, space left %i, size : %zu \n", free_space_left_in_chunk, size);
		return (alloc_addr);
	}
	if (!available_chunk->next) {
//		if (current_pages->total_bytes_free < size) {
//			return (NULL);
//		}
//		ici sizeof(t_chunk) est la taille du chunk availble qu'on a fill (qui n'est pas dispo)
		add_back(free_space_left_in_chunk, NULL, current_pages);
	} else {
		//insert entre le chunk et le suivant le reste du chunk
		//ft_printf("test %i %i %i %i\n", free_space_last_chunk, free_space_last_chunk - (sizeof(t_chunk) + size), 100000, size);
		insert_free_chunk_in_between(free_space_last_chunk - (size + sizeof(t_chunk)), (void *) alloc_addr + available_chunk->size, current_pages, available_chunk);
	}
	////ft_printf("%p\n", alloc_addr);k
	//split le chunk si le next n'est pas null
	//return (current_pages->chunks->chunk_addr + sizeof(t_chunk));
	return (alloc_addr);
}

void	insert_free_chunk_in_between(size_t free_space, void *addr, t_pages *current_page, t_chunk *base_chunk) {

	int	alignement_needed = (size_t)(addr) % 8;    
	if (free_space <= sizeof(t_chunk) + alignement_needed) {
		//soit faire le cas valeur absolue on soutrait au lieux d'ajouter pour obtenir l'
		//ft_printf("%i %i\n", free_space, alignement_needed + sizeof(t_chunk));
		return ;
	}

	t_chunk *next_chunk = base_chunk->next;
	base_chunk->next = (t_chunk *)((void *)(addr + alignement_needed));
	t_chunk *new = base_chunk->next;
	new->size = free_space - sizeof(t_chunk) - alignement_needed;
	new->next = next_chunk;
	new->alignement = alignement_needed;
	new->state = FREE;
	new->page_type = current_page->type;
	current_page->free_chunks += 1;
	current_page->total_bytes_alignement += alignement_needed;
	//current_page->total_bytes_free += new->size;
	//ft_printf("%p %p new size %zu\n", next_chunk, base_chunk->next, new->size);
}

t_pages	*get_ptr_page(void *ptr) {

	t_chunk *chunk_info = ptr - sizeof(t_chunk);
	switch (chunk_info->page_type) {
		case 0:
			return (&alloc_info.tiny);
		case 1:
			return (&alloc_info.small);
		case 2:
			return (&alloc_info.large);
		default:
			return (NULL);
	}
}

//Verifie l'existence du chunk dans la page (dans la liste chaine)

//On peux rajouter le merge, si on a un free on check si le suivant est free
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
	//ft_printf("TYPE PAGE TO FREE : %i %zu\n", ptr_page->type, ptr_page->busy_chunks);
	t_chunk *chunk_to_free = get_chunk(ptr, ptr_page);
	////ft_printf("test %p %ld\n", chunk_to_free, chunk_to_free->size);
	if (!chunk_to_free) {
		return ;
	}
	//fonction de changement d'etat ? 
	chunk_to_free->state = FREE;
	//ptr_page->total_bytes_free += chunk_to_free->size;
	ptr_page->total_bytes_busy -= chunk_to_free->size;
	ptr_page->free_chunks += 1;
	ptr_page->busy_chunks -= 1;
	////ft_printf("GROUUU %ld %ld %ld\n", ptr_page->total_bytes_free, chunk_to_free->size,ptr_page->busy_chunks);
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
	//ft_printf("%p %zu call realooc\n", ptr, size);
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
	//ft_printf("testbis\n");
	t_pages *ptr_page = get_ptr_page(ptr);
	if (!ptr_page) {
		return (res);
	}
	//ft_printf("test1\n");
	t_chunk *info_ptr = get_chunk(ptr, ptr_page);
	//ft_printf("test\n");
	if (info_ptr) {
		res = ft_malloc(size);
		//ft_printf("size ptr %zu %p\n", info_ptr->size, info_ptr);
		ft_memcpy(res, ptr, size);
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
	size_t	free_chunks = 0;
	size_t	free_bytes = 0;
	while (chunk) {
		next = chunk->next;
		if (chunk->state == BUSY) {
			//ft_printf("%p - %p: %lu bytes\n", chunk, next, chunk->size);
			alloced_bytes += chunk->size;
		} else {
			//ft_printf("FREE ? %p - %p: %lu bytes\n", chunk, next, chunk->size);
			free_bytes += chunk->size;
			free_chunks += 1;
		}
		chunk = next;
	}
	//ft_printf("free total = %zu in memory %zu\nfree_chunks :%zu in memory, real : %zu\n", free_bytes, pages->total_bytes_free, pages->free_chunks, free_chunks);
	return alloced_bytes;
}

void	show_alloc_mem() {
	size_t	alloced_bytes = 0;
	//ft_printf("%s %p\n", "TINY:", alloc_info.tiny.chunks);
	alloced_bytes += browse_chunks(&alloc_info.tiny);
	//ft_printf("%s %p\n", "SMALL:", alloc_info.small.chunks);
	alloced_bytes += browse_chunks(&alloc_info.small);
	//ft_printf("%s %p\n", "LARGE:", alloc_info.large.chunks);
	alloced_bytes += browse_chunks(&alloc_info.large);
	//ft_printf("Total alloc: %zu bytes\n", alloced_bytes);
}

#ifdef DYNAMIC

//#warning "DYNAMIC is defined"
void    *malloc(size_t size)
{
    ft_putstr_fd("malloc\n", STDOUT_FILENO);
    void *ptr = ft_malloc(size);
	//show_alloc_mem();
	//ft_printf("%p returned\n", ptr);
	return (ptr);
}
void    free(void *ptr)
{
    ft_putstr_fd("free\n", STDOUT_FILENO);
    ft_free(ptr);
}

void	*calloc(size_t nmemb, size_t size) {
    ft_putstr_fd("calloc\n", STDOUT_FILENO);
	void *ptr = ft_malloc(size * nmemb);
	if (!ptr) {
		return (NULL);
	}
	ft_memset(ptr, 0, nmemb * size);
	return (ptr);
}

void    *realloc(void *ptr, size_t size)
{
    ft_putstr_fd("realloc\n", STDOUT_FILENO);
	void *ptr2 = ft_realloc(ptr, size);
	//show_alloc_mem();
	return (ptr2);

}
*/
#endif
