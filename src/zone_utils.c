#include "ft_malloc.h"


void	new_mem_zone(t_mem_zone *zone, t_type type) {
	zone->free_chunks = 1;
	zone->busy_chunks = 0;
	zone->next = NULL;
	zone->zone_type = type;
	zone->first = new_chunk((void *)(zone) + sizeof(t_mem_zone), FREE, type, (type == TINY ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE) - sizeof(t_chunk) - sizeof(t_mem_zone));
	zone->largest_chunk = zone->first;
}

void	*ask_for_mem_zone(t_type type, size_t size) {
	void	*zone = NULL;

	if (type == TINY) {
		zone = mmap(g_alloc_info.tiny, TINY_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	} else if (type == SMALL) {
		zone = mmap(g_alloc_info.small, SMALL_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	} else {
		zone = mmap(g_alloc_info.large, size + sizeof(t_mem_zone),PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	}
	return (zone);
}

t_type	get_zone_type(size_t size) {
	t_type matching_type = TINY;
	if (size >= 4096) {
		matching_type = LARGE;
	} else if (size >= 560) {
		matching_type = SMALL;
	}
	return (matching_type);
}

bool	loop_on_zone(void *ptr, t_mem_zone **finded_zone, t_mem_zone *zone) {
	void *zone_ptr;
	while (zone) {
		zone_ptr = zone->first;
		if (ptr >= zone_ptr && ptr <= zone_ptr + zone->size) {
			*finded_zone = zone;
			return (true);
		}
		zone = zone->next;
	}
	return (false);
}

bool	get_ptr_zone(void *ptr, t_mem_zone **finded_zone) {
	if (!loop_on_zone(ptr, finded_zone, g_alloc_info.tiny)
		&& !loop_on_zone(ptr, finded_zone, g_alloc_info.small)
		&& !loop_on_zone(ptr, finded_zone, g_alloc_info.large))
	{
		return (false);
	}
	return (true);
}

