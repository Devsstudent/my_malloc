#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

# include "my_secmalloc.h"
# include <stdio.h>
# include <alloca.h>
# include <unistd.h>
# define _GNU_SOURCE
# include <sys/mman.h>
# include <stdbool.h>
# include <string.h>
# include <errno.h>
# include <stdint.h>
# include <time.h>
# include <stdlib.h>
# include <stdarg.h>
# include <fcntl.h>
# include <stdbool.h>



# define PAGE_SIZE 4096

typedef enum chunk_state {
	FREE,
	BUSY,
}	t_chunk_state;

typedef struct chunk {
	void			*data_addr;
	t_chunk_state	state;
	size_t			size;
	struct chunk	*next;
	struct chunk	*prev;
	uint64_t	canary;
}	t_chunk;


typedef struct info_malloc {
	size_t	nb_data_pages;
	size_t	nb_meta_pages;
	size_t	nb_chunks;
	t_chunk	*first_chunk;
	t_chunk	*last_chunk;
	size_t	total_data_bytes;
	size_t	total_meta_bytes;
}	t_info_malloc;

typedef struct data {
	size_t pages;
	size_t total;
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
t_chunk	*new_chunk(void *data_addr, size_t size, t_chunk_state state);
void	addLog(char *format, ...);

#endif
