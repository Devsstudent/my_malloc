#include "ft_malloc.h"

bool	check_chunk_is_matching(t_chunk *chunk, size_t size, t_mem_zone *current_zone) {
	bool state = false;
	if (chunk && chunk->size >= size && chunk->state == FREE) {
		split_chunk(chunk, current_zone, size);
		state = true;
	}
	return (state);
}

void	split_chunk(t_chunk *chunk_to_split, t_mem_zone *current_zone, size_t size) {
	ft_printf("SPLITIIII");
	if (chunk_to_split->size == size) {
		chunk_to_split->state = BUSY;
		current_zone->busy_chunks += 1;
		current_zone->free_chunks -= 1;
	} else {
		chunk_to_split->state = BUSY;
		int size_new_chunk = chunk_to_split->size - size;
		if (size_new_chunk - (int)sizeof(t_chunk) > 0) {
			t_chunk *new = new_chunk((void *)(chunk_to_split) + size + sizeof(t_chunk), FREE, current_zone->zone_type, size_new_chunk - sizeof(t_chunk));
			new->prev = chunk_to_split;
			if (chunk_to_split->next) {
				new->next = chunk_to_split->next;
				chunk_to_split->next->prev = new;
			}
			chunk_to_split->next = new;
			current_zone->busy_chunks += 1;
		} else {
			current_zone->free_chunks -= 1;
		}
		chunk_to_split->size = size;
	}
	if (current_zone->largest_chunk == chunk_to_split) {
		current_zone->largest_chunk = find_largest_chunk(current_zone);
	}
}

t_chunk *find_largest_chunk(t_mem_zone *current_zone) {
	t_chunk	*buff = current_zone->first;
	t_chunk	*new_largest_chunk = NULL;
	size_t	max = 0;
	while (buff) {
		if (buff->size > max && buff->state == FREE) {
			new_largest_chunk = buff;
			max = buff->size;
		}
		buff = buff->next;
	}
	return (new_largest_chunk);
}

t_chunk *get_chunk(t_mem_zone *current_zone, size_t size) {
	t_chunk *matching_chunk = NULL;

	t_chunk *buff = current_zone->first;
	if (check_chunk_is_matching(buff, size, current_zone))
		matching_chunk = buff;

	while (buff && !matching_chunk) {
		if (check_chunk_is_matching(buff, size, current_zone))
			matching_chunk = buff;
		buff = buff->next;
	}
	return (matching_chunk);
}

t_chunk *new_chunk(void *chunk_addr, t_state state, t_type zone_type, size_t size) {
	t_chunk *new_chunk = chunk_addr;
	new_chunk->size = size;
	new_chunk->state = state;
	new_chunk->zone_type = zone_type;
	new_chunk->next = NULL;
	new_chunk->prev = NULL;
	return (new_chunk);
}

//get_chunk(t_mem_zone *mem_zone, void *addr ici addr - sizeof(t_chunk) est l'address a match)
bool	get_ptr_chunk(void *ptr, t_mem_zone *ptr_mem_zone, t_chunk **ptr_chunk) {
	t_chunk *buff = ptr_mem_zone->first;
	while (buff) {
		if (buff == ptr) {
			*ptr_chunk = buff;
			return (true);
		}
		buff = buff->next;
	}
	return (false);
}
