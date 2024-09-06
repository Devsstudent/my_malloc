#ifndef MALLOC_H
# define MALLOC_H

# include <stddef.h>
# include <sys/mman.h>
# include <stdbool.h>
# include <unistd.h>
# include "libft.h"
# define PAGE_SIZE getpagesize()

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
	t_type				page_type;
	struct chunk		*next;
	struct chunk		*prev;
}	t_chunk;

typedef struct mem_zone {
	size_t			free_chunks;
	size_t			busy_chunks;
	struct mem_zone	*next;
	t_chunk			*first;
	t_type			zone_type;
	size_t			max_size_available;
}	t_mem_zone;

typedef struct tiny {
	t_mem_zone		*tiny;
}	t_tiny;

typedef struct small {
	t_mem_zone		*small;
}	t_small;

typedef struct large {
	t_mem_zone		*large;
}	t_large;

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

extern t_alloc_info *get_info(void);
void	*ft_malloc(size_t size);
void	*ft_realloc(void *ptr, size_t size);
void	ft_free(void *ptr);
void	insert_free_chunk_in_between(size_t free_space, void *addr, t_pages *current_page, t_chunk *base_chunk);
extern t_chunk *looking_for_chunk(t_pages *page, size_t size);
extern t_chunk *get_chunk(void *ptr, t_pages *ptr_page);

extern	void	show_alloc_mem();

#endif
