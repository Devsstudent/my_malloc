/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_bis.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 22:04:15 by odessein          #+#    #+#             */
/*   Updated: 2024/09/07 14:48:13 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_alloc_info g_alloc_info;

void *ft_malloc(size_t size) {
	// align the size
	size = (size + 15) & ~15;

	// 1 - Get the current zone
	t_mem_zone *current_zone = get_current_zone(size);

	if (!current_zone) {
		//Error on a crash
		return NULL;
	}

	t_chunk *asked_chunk = get_chunk(current_zone, size);

	if (!asked_chunk) {
		//Error on a pas trouver de chunk qui match (ca doit jamais arriver)
		return NULL;
	}
	return ((void *)(asked_chunk) + sizeof(t_chunk));

	// 2 - We have the zone, so we just have to create the chunk inside it
	//

	// - Problematique : la max_size_available, c'est soit les chunks free, donc faut check au free
	// soit c'est le dernier chunk je pense (je vais garder la logique du add_back facile)
	// get_chunk

	// 3 - We return the chunk

}

bool	check_chunk_is_matching(t_chunk *chunk, size_t size, t_mem_zone *current_zone) {
	bool state = false;
	if (chunk->size >= size && chunk->state == FREE) {
		split_chunk(chunk, current_zone, size);
		state = true;
	}
	return (state);
}

//ici current zone just pour adapter la max size available
void	split_chunk(t_chunk *chunk_to_split, t_mem_zone *current_zone, size_t size) {
//	bool state = false;
	if (chunk_to_split->size == size) {
		chunk_to_split->state = BUSY;
//		state = true;
	} else {
		chunk_to_split->state = BUSY;
		size_t size_new_chunk = chunk_to_split->size - size;
		if (size_new_chunk - sizeof(t_chunk) > 0) {
		//	split
			
			t_chunk *new = new_chunk((void *)(chunk_to_split + size), FREE, current_zone->zone_type, size_new_chunk - sizeof(t_chunk));
			new->prev = chunk_to_split;
			if (chunk_to_split->next) {
				new->next = chunk_to_split->next;
				chunk_to_split->next->prev = new;
			}
			chunk_to_split->next = new;
//			state = true;
		}
	}
	if (current_zone->largest_chunk == chunk_to_split) {
		current_zone->largest_chunk = find_largest_chunk(current_zone);
		//find_another_one, or null if there is no dans le cas des larges genre
	}
	//return (state);
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

t_mem_zone	*get_current_zone(size_t size) {
	t_type current_zone_type = get_zone_type(size);

	t_mem_zone	*current_zone = NULL;

	if (current_zone_type == TINY || current_zone_type == SMALL) {
		current_zone = get_tiny_or_small_zone(current_zone_type, size);
	} else {
		//dans le cas ou on a un block qui est free
		current_zone = look_for_matching_zone(g_alloc_info.large, size);
		if (!current_zone)
		{
			current_zone = ask_for_mem_zone(current_zone_type, size + sizeof(t_chunk));
			if (current_zone) {
				current_zone->zone_type = current_zone_type;
				current_zone->free_chunks = 1;
				current_zone->busy_chunks = 0;
				current_zone->next = NULL;
				current_zone->first = new_chunk((void *)(current_zone) + sizeof(t_mem_zone), FREE, current_zone_type, size - sizeof(t_chunk));
				current_zone->largest_chunk = current_zone->first;
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
	}
	while (large_zone && large_zone->next) {
		large_zone = large_zone->next;
	}
	large_zone->next = zone;
	g_alloc_info.nb_large_elems += 1;
}

//C'est important de poser comment le sizeof(t_chunk) est pris en compte
//J'ai decider de ne pas le deduire pour les larges dans la size_availble pour trouver des page qui match
t_mem_zone	*look_for_matching_zone(t_mem_zone *zone, size_t size) {
	t_mem_zone	*matching_zone = NULL;

	while (zone) {
		if (zone->largest_chunk->size <= size) {
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
			//en theorie a ce moment faut aussi cree le chunk free
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
	} else {
		g_alloc_info.last_small = new_zone;
	}
	return (state);
}

void	new_mem_zone(t_mem_zone *zone, t_type type) {
	zone->free_chunks = 1;
	zone->busy_chunks = 0;
	zone->next = NULL;
	zone->zone_type = type;
	zone->first = new_chunk((void *)(zone) + sizeof(t_mem_zone), FREE, type, (type == TINY ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE) - sizeof(t_chunk) - sizeof(t_mem_zone));
	zone->largest_chunk = zone->first;
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
