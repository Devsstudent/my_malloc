#include "malloc.h"

void	merge_with_next(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone) {
	t_chunk	*next = NULL;
	t_chunk *chunk_freed = *ptr_chunk;

	if (chunk_freed->next->size > 0) {
		chunk_freed->size += chunk_freed->next->size + sizeof(t_chunk);
		next = chunk_freed->next->next;
		ft_memset(chunk_freed->next, 0, chunk_freed->next->size + sizeof(t_chunk));
		if (next) {
			next->prev = chunk_freed;
		}
		ptr_mem_zone->free_chunks -= 1;
		chunk_freed->next = next;
		ptr_mem_zone->largest_chunk = find_largest_chunk(ptr_mem_zone);
	}
}

void	merge_chunk(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone) {

	t_chunk *chunk_freed = *ptr_chunk;

	if (chunk_freed && chunk_freed->prev && chunk_freed->prev->state == FREE) {
		merge_with_prev(ptr_chunk, ptr_mem_zone, ptr_chunk);
	}
	if (chunk_freed && chunk_freed->next && chunk_freed->next->state == FREE) {
		merge_with_next(ptr_chunk, ptr_mem_zone);
	}
}

bool	valid_ptr(t_mem_zone **ptr_mem_zone, t_chunk **ptr_chunk, void *ptr) {
	bool	state = false;

	if (ptr && get_ptr_zone(ptr, ptr_mem_zone))
	{
		if (get_ptr_chunk(ptr - sizeof(t_chunk), *ptr_mem_zone, ptr_chunk)) {
				state = true;
		} else {
			char *err = "Error getting ptr_chunk\n";
			write(2, err, ft_strlen(err));
		}
	} else {
		char *err = "Error getting ptr_zone\n";
		write(2, err, ft_strlen(err));
	}
	return state;
}

size_t	get_available_size(t_chunk *next, size_t current_ptr_size) {
	size_t	available_size;

	available_size = 0;
	if (next && next->state == FREE) {
		available_size = next->size + current_ptr_size;
	}

	return available_size;
}
