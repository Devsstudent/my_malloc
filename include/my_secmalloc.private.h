#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

# include "my_secmalloc.h"
# include <stdio.h>
# include <alloca.h>
# include <unistd.h>
# include <sys/mman.h>
# include <stdbool.h>
# include <string.h>
# include <errno.h>

# define PAGE_SIZE 4096

typedef enum chunk_state {
	FREE,
	BUSY,
}	t_chunk_state;

typedef struct chunk {
	void			*data_addr;
	t_chunk_state	state;
	size_t			size;
	size_t			bytes_before;
	struct chunk	*next;
	struct chunk	*prev;
}	t_chunk;

typedef struct data {
	size_t pages;
}	t_data;

typedef struct metadata {
	size_t	chunks_nb;
	size_t	pages;
	t_chunk	*first_chunk;
	t_chunk	*last_chunk;
}	t_metadata;


// sentez vous libre de modifier ce header comme vous le souhaitez

void    *my_malloc(size_t size);
void    my_free(void *ptr);
void    *my_calloc(size_t nmemb, size_t size);
void    *my_realloc(void *ptr, size_t size);
void	fill_chunk(size_t byte_before, size_t size, t_chunk_state state, t_chunk *new);

#endif
