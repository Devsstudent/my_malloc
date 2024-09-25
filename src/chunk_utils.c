#include "malloc.h"

void	merge_with_next(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone, t_chunk **base) {
	t_chunk	*new_chunk = NULL;
	t_chunk *chunk_freed = *ptr_chunk;

	if (!new_chunk) {
		new_chunk = chunk_freed;
	}
	new_chunk->size += chunk_freed->next->size + sizeof(t_chunk);
	ptr_mem_zone->free_chunks -= 1;

	t_chunk *next = chunk_freed->next;

		printf("allllo %p\n", next);
	if (next && next->next) {
		printf("%p\n", next->next);
		new_chunk->next = next->next;
		next->next->prev = new_chunk;
	}
}

void	merge_chunk(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone) {

	t_chunk	*new_chunk = NULL;
	t_chunk *chunk_freed = *ptr_chunk;

	if (chunk_freed->prev && chunk_freed->prev->state == FREE) {
		merge_with_prev(ptr_chunk, ptr_mem_zone, &new_chunk);
	}
	if (chunk_freed->next && chunk_freed->next->state == FREE) {
		merge_with_next(ptr_chunk, ptr_mem_zone, &new_chunk);
	}
	if (new_chunk) {
		*ptr_chunk = new_chunk;
	}
}

bool	valid_ptr(t_mem_zone **ptr_mem_zone, t_chunk **ptr_chunk, void *ptr) {
	bool	state = false;

//	pthread_mutex_lock(&mutex_malloc);
	if (ptr && get_ptr_zone(ptr, ptr_mem_zone))
	{
		if (get_ptr_chunk(ptr - sizeof(t_chunk), *ptr_mem_zone, ptr_chunk)) {
				state = true;
		} else {
			ft_printf("Error getting ptr_chunk\n");
		}
	} else {
		ft_printf("Error getting ptr_zone\n");
	}
//	pthread_mutex_unlock(&mutex_malloc);
	return state;
}

size_t	get_available_size(t_chunk *next, size_t current_ptr_size) {
	size_t	available_size;

	available_size = 0;
	if (next && next->state == FREE) {
		available_size = next->size + current_ptr_size;
	}
	//ft_printf("next->state %i\n", next->state);

	return available_size;
}

