#pragma once

# include <stddef.h>
# include <sys/mman.h>
# include <stdbool.h>
# include <unistd.h>
# include "libft.h"
# include <pthread.h>
# define PAGE_SIZE getpagesize()
# define TINY_ZONE_SIZE (560 * 100 + 100 * sizeof(t_chunk)) + sizeof(t_mem_zone)
# define SMALL_ZONE_SIZE (4096 * 100 + 100 * sizeof(t_chunk)) + sizeof(t_mem_zone)

typedef enum type {
	TINY,
	SMALL,
	LARGE
}	t_type;

typedef enum state {
	FREE,
	BUSY,
}	t_state;

typedef struct chunk {
	size_t				size;
	t_state				state;
	t_type				zone_type;
	struct chunk		*next;
	struct chunk		*prev;
}	t_chunk;

typedef struct mem_zone {
	size_t			size;
	size_t			free_chunks;
	size_t			busy_chunks;
	struct mem_zone	*next;
	t_chunk			*first;
	t_type			zone_type;
	t_chunk			*largest_chunk;
}	t_mem_zone;

typedef struct alloc_info {
	// < 560
	t_mem_zone		*tiny;
	t_mem_zone		*last_tiny;
	size_t		nb_tiny_elems;
	// < 4096
	t_mem_zone		*small;
	t_mem_zone		*last_small;
	size_t		nb_small_elems;
	// > 4096
	t_mem_zone		*large;
	t_mem_zone		*last_large;
	size_t		nb_large_elems;
}	t_alloc_info;

t_type	get_zone_type(size_t size);
void	*ask_for_mem_zone(t_type type, size_t size);
t_chunk *new_chunk(void *chunk_addr, t_state state, t_type zone_type, size_t size);
void	new_mem_zone(t_mem_zone *zone, t_type type);
bool add_zone_tiny_small(t_mem_zone *mem_zone, t_type zone_type);
t_mem_zone *get_tiny_or_small_zone(t_type type, size_t size);
t_mem_zone	*look_for_matching_zone(t_mem_zone *zone, size_t size);
void	add_large_zone(t_mem_zone *zone);
t_mem_zone	*get_current_zone(size_t size);
t_chunk *find_largest_chunk(t_mem_zone *current_zone);
t_chunk *get_chunk(t_mem_zone *current_zone, size_t size);
void	split_chunk(t_chunk *chunk_to_split, t_mem_zone *current_zone, size_t size);
bool	check_chunk_is_matching(t_chunk *chunk, size_t size, t_mem_zone *current_zone);
bool	get_ptr_chunk(void *ptr, t_mem_zone *ptr_mem_zone, t_chunk **ptr_chunk);
bool	loop_on_zone(void *ptr, t_mem_zone **finded_zone, t_mem_zone *zone);
bool	get_ptr_zone(void *ptr, t_mem_zone **finded_zone);
void	merge_chunk(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone);
bool	valid_ptr(t_mem_zone **ptr_mem_zone, t_chunk **ptr_chunk, void *ptr);
void	merge_chunk(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone);
void	merge_with_next(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone, t_chunk **base);
void	merge_with_prev(t_chunk **ptr_chunk, t_mem_zone *ptr_mem_zone, t_chunk **base);
size_t	get_available_size(t_chunk *next, size_t current_ptr_size);

extern t_alloc_info get_alloc_info();

t_alloc_info *get_info(void);
void	*ft_malloc(size_t size);
void	*ft_realloc(void *ptr, size_t size);
void	ft_free(void *ptr);

extern	void	show_alloc_mem();

extern pthread_mutex_t mutex_malloc;

extern t_alloc_info g_alloc_info;
