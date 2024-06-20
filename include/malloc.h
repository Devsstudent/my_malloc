#ifndef MALLOC_H
# define MALLOC_H

#include <stddef.h>
#include "libft.h"

typedef struct pages {
	size_t	nb_blocks;
	void *last_addr;
	void *first_addr;
}	t_pages;

typedef struct free_elem {
	size_t				size;
	void				*chunk_addr;
	struct free_elem	*next ;
	//Prev ?
}	t_free_elem;

typedef struct li_free {
	size_t	length;
	t_free_elem	*first;
}	t_li_free;

typedef struct alloc_info {
	t_pages		tiny;
	t_pages		small;
	t_pages		largs;
	t_li_free	li_free;
}	t_alloc_info;

#endif
