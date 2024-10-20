#include "malloc.h"

t_mem_zone	*get_current_zone(size_t size) {
	t_type current_zone_type = get_zone_type(size);

	t_mem_zone	*current_zone = NULL;

	if (current_zone_type == TINY || current_zone_type == SMALL) {
		current_zone = get_tiny_or_small_zone(current_zone_type, size);
	} else {
		current_zone = look_for_matching_zone(g_alloc_info.large, size);
		if (!current_zone)
		{
			current_zone = ask_for_mem_zone(current_zone_type, ROUND_UP_TO_PAGE_SIZE(size + sizeof(t_chunk)));
			if (current_zone) {
				current_zone->zone_type = current_zone_type;
				current_zone->free_chunks = 1;
				current_zone->busy_chunks = 0;
				current_zone->next = NULL;
				current_zone->first = new_chunk((void *)(current_zone) + sizeof(t_mem_zone), FREE, current_zone_type, size);
				current_zone->largest_chunk = current_zone->first;
				current_zone->size = ROUND_UP_TO_PAGE_SIZE(size + sizeof(t_chunk)) - sizeof(t_chunk);
				add_large_zone(current_zone);
			}
		}
	}

	return current_zone;
}

void	add_large_zone(t_mem_zone *zone) {
	t_mem_zone *large_zone = g_alloc_info.large;
	if (!large_zone) {
		g_alloc_info.large = zone;
		g_alloc_info.last_large = zone;
	} else {
		while (large_zone && large_zone->next) {
			large_zone = large_zone->next;
		}
		large_zone->next = zone;
		g_alloc_info.last_large = zone;
	}
	g_alloc_info.nb_large_elems += 1;
}

//C'est important de poser comment le sizeof(t_chunk) est pris en compte
//J'ai decider de ne pas le deduire pour les larges dans la size_availble pour trouver des page qui match
t_mem_zone	*look_for_matching_zone(t_mem_zone *zone, size_t size) {
	t_mem_zone	*matching_zone = NULL;

	t_mem_zone	*buff = zone;

	while (buff) {
		if (buff->largest_chunk && buff->largest_chunk->size >= size) {
			matching_zone = buff;
			break ;
		}
		buff = buff->next;
	}
	return matching_zone;
}

t_mem_zone *get_tiny_or_small_zone(t_type type, size_t size) {
	t_mem_zone *zone = NULL;

	if (type == TINY) {
		zone = look_for_matching_zone(g_alloc_info.tiny, size);
		if (!zone && add_zone_tiny_small(g_alloc_info.tiny, type)) {
			zone = g_alloc_info.last_tiny;
			g_alloc_info.nb_tiny_elems += 1;
		}
	} else {
		zone = look_for_matching_zone(g_alloc_info.small, size);
		if (!zone && add_zone_tiny_small(g_alloc_info.small, type)) {
			zone = g_alloc_info.last_small;
			g_alloc_info.nb_small_elems += 1;
		};
	}
	return (zone);
}

bool add_zone_tiny_small(t_mem_zone *mem_zone, t_type zone_type) {
	bool state = false;
	t_mem_zone *new_zone = NULL;

	if (!mem_zone) {
		mem_zone = ask_for_mem_zone(zone_type, 0);
		if (mem_zone) {
			state = true;
			new_mem_zone(mem_zone, zone_type);
			new_zone = mem_zone;
			if (zone_type == TINY)
				g_alloc_info.tiny = new_zone;
			else
				g_alloc_info.small = new_zone;
		}
	}
	//Meaning  on est pas passer au dessus
	if (!state && mem_zone) {
		new_zone = ask_for_mem_zone(zone_type, 0);
		if (mem_zone) {
			state = true;
			while (mem_zone && mem_zone->next) {
				mem_zone = mem_zone->next;
			}
			mem_zone->next = new_zone;
			new_mem_zone(new_zone, zone_type);
		}
	}
	if (zone_type == TINY) {
		g_alloc_info.last_tiny = new_zone;
		new_zone->size = TINY_ZONE_SIZE - sizeof(t_mem_zone);
	} else {
		g_alloc_info.last_small = new_zone;
		new_zone->size = SMALL_ZONE_SIZE - sizeof(t_mem_zone);
	}
	return (state);
}
