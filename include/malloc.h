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
	void				*chunk_addr;
	struct chunk		*next ;
	//Prev ?
}	t_chunk;

typedef struct pages {
	size_t		free_chunks;
	size_t		busy_chunks;
	size_t		total_bytes_busy;
	size_t		total_bytes_free;
	size_t		total_bytes_alignement;
	t_chunk		*chunks;
	t_chunk		*last;
	size_t		page_nb;
	t_type		type;
}	t_pages;

typedef struct alloc_info {
	// < 560
	t_pages			tiny;
	// < 4096
	t_pages			small;
	// > 4096
	t_pages			large;
}	t_alloc_info;


extern t_alloc_info *get_info(void);
void	*ft_malloc(size_t size);
void *ft_realloc(void *ptr, size_t size);
void ft_free(void *ptr);
extern t_chunk *looking_for_chunk(t_pages *page, size_t size);

#endif
