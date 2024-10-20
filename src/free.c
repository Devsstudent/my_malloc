#include "malloc.h"

static inline void free_large_zone(t_mem_zone *ptr_mem_zone) {
	t_mem_zone *buff = g_alloc_info.large;
	t_mem_zone *prev = NULL;
	while (buff) {
		if (buff->next && buff->next == ptr_mem_zone) {
			prev = buff;
		}
		buff = buff->next;
	}
	if (prev) {
		prev->next = ptr_mem_zone->next;
	}
	if ((void *)ptr_mem_zone + sizeof(t_mem_zone) == g_alloc_info.large->first) {
		g_alloc_info.large->first = ptr_mem_zone->first->next;
	}
	if (!g_alloc_info.large->first) {
		g_alloc_info.large = NULL;
	}
	if (munmap(ptr_mem_zone, ptr_mem_zone->size) < 0) {
		write(2, "Error munamp\n", ft_strlen("Error munmap\n"));
	}
}

void ft_free(void *ptr) {
	//ft_printf("Free %p\n", ptr);
	t_mem_zone	*ptr_mem_zone = NULL;
	t_chunk		*ptr_chunk = NULL;

	//Check la memzone
	pthread_mutex_lock(&mutex_malloc);
	bool	isValidPtr = (ptr != NULL && valid_ptr(&ptr_mem_zone, &ptr_chunk, ptr));
	if (isValidPtr && ptr_chunk->state == FREE) {
		char *err = "Error double free\n";
		write(2, err, ft_strlen(err));
	} else if (isValidPtr) {
	//	pthread_mutex_lock(&mutex_malloc);
		ptr_chunk->state = FREE;
		ptr_mem_zone->free_chunks += 1;
		if (ptr_mem_zone->busy_chunks > 0) {
			ptr_mem_zone->busy_chunks -= 1;
		}

		merge_chunk(&ptr_chunk, ptr_mem_zone);


		if (ptr_chunk->zone_type == LARGE) {
			free_large_zone(ptr_mem_zone);
		}
	}
	pthread_mutex_unlock(&mutex_malloc);
}

void	merge_with_prev(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone, t_chunk **first) {

	t_chunk	*new_chunk;
	t_chunk *chunk_freed = *ptr_chunk;

	new_chunk = chunk_freed->prev;
	new_chunk->size += chunk_freed->size + sizeof(t_chunk);
	ptr_mem_zone->free_chunks -= 1;
	t_chunk *next = chunk_freed->next;
	new_chunk->next = next;
	if (next) {
		next->prev = new_chunk;
	}
	*first = new_chunk;
	ptr_mem_zone->largest_chunk = find_largest_chunk(ptr_mem_zone);
}
