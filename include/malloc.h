#ifndef MALLOC_H
# define MALLOC_H

# include <stddef.h>
# include <sys/mman.h>
# include <stdbool.h>
# include <unistd.h>
# include "libft.h"
# define PAGE_SIZE getpagesize()


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

typedef struct li_chunks{
	size_t		free_chunks;
	size_t		busy_chunks;
	size_t		total_bytes_busy;
	size_t		total_bytes_free;
	t_chunk		*next;
}	t_li_chunks;

typedef struct pages {
	t_li_chunks		li_chunks;
	size_t			page_nb;
	void			*last_addr;
	void			*first_addr;
}	t_pages;

typedef struct alloc_info {
	// < 560
	t_pages			tiny;
	// < 4096
	t_pages			small;
	// > 4096
	t_pages			large;
}	t_alloc_info;


void	*ft_malloc(size_t size);
void *ft_realloc(void *ptr, size_t size);
void ft_free(void *ptr);

#endif
