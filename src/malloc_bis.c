/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_bis.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 22:04:15 by odessein          #+#    #+#             */
/*   Updated: 2024/09/06 22:30:54 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_type	get_zone_type(size_t size);

t_alloc_info g_alloc_info = 0;

void *ft_malloc(size_t size) {
	// align the size
	size = (size + 15) & ~15;

	// 1 - Get the current zone
	t_mem_zone current_zone = get_current_zone(size);

	// 2 - We have the zone, so we just have to create the chunk inside it

	// 3 - We return the chunk

}

t_mem_zone	*get_current_zone(size_t size) {
	t_type current_zone_type = get_zone_type(size);

	t_mem_zone	current_zone = NULL;

	if (current_zone_type == TINY || current_zone_type == SMALL) {
		current_zone = get_tiny_small_zone(current_zone_type, size);
	} else {
		//Donc faut toujours ask for mem zone large
		//sachant que pour les larges, il faut toujours add_back la zone

		//
		//so check if the zone have enough space first
		//If we need a new mem_zone then : may set the mem_zone add back, initialize etc
		current_zone = ask_for_mem_zone(current_zone_type, size);
	}
	return current_zone;
}

t_mem_zone	*look_for_matching_zone(t_mem_zone *zone, size_t size) {
	t_mem_zone	*matching_zone = NULL;

	while (zone) {
		if (zone->max_size_availbale <= size + sizeof(t_chunk)) {
			matching_zone = zone;
			break ;
		}
		zone = zone->next;
	}
	return matching_zone;
}

t_mem_zone *get_tiny_or_small_zone(t_type type, size_t size) {
	t_mem_zone *zone = NULL;

	if (type == TINY) {
		zone = look_for_matching_zone(g_alloc_info->tiny, size);
		if (!zone && add_zone_tiny_small(g_alloc_info->tiny, type)) {
			zone = g_alloc_info->last_tiny;
			g_alloc_info->nb_tiny_elems += 1;
		}
	} else (type == SMALL) {
		zone = look_for_matching_zone(g_alloc_info->small, size);
		if (!zone && add_zone_tiny_small(g_alloc_info->small, type)) {
			zone = g_alloc_info->last_small;
			g_alloc_info->nb_small_elems += 1;
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
			//en theorie a ce moment faut aussi cree le chunk free
			state = true;
			new_mem_zone(mem_zone, zone_type);
			new_zone = mem_zone;
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
			//en theorie a ce moment faut aussi cree le chunk free
			//fill les info de la struct, faire une function new_mem_zone genre
			mem_zone->next = new_zone;
			new_mem_zone(new_zone, zone_type);
			}
	}
	if (type == TINY) {
		g_alloc_info->last_tiny = new_zone;
	} else {
		g_alloc_info->last_small = new_zone;
	}
	return (state);
}

void	new_mem_zone(t_mem_zone *zone, t_type type) {
	zone->free_chunks = 1;
	zone->busy_chunks = 0;
	zone->next = NULL;
	zone->page_type = type;
	zone->max_size_availbale = (type == TINY ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE) - sizeof(t_chunk) - sizeof(t_mem_zone);
	zone->first = new_chunk(zone + sizeof(t_mem_zone), FREE, type, zone->max_size_availbale);
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


void	*ask_for_mem_zone(t_type type, size_t size) {
	void	*zone = NULL;
	if (type == TINY) {
		zone = mmap(g_alloc_info->tiny, TINY_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	} else if (type == SMALL) {
		zone = mmap(g_alloc_info->small, SMALL_ZONE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	} else {
		zone = mmap(g_alloc_info->large, size + sizeof(t_mem_zone),PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
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
