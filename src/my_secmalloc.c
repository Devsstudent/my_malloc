#define _GNU_SOURCE
#include "my_secmalloc.private.h"

void	*heap_data;
void	*heap_metadata;
t_metadata	meta;
t_data		data;

bool	init_heap(void) {
	printf("INIT\n");
	heap_data = mmap((void *)(4096 * 100000), PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_data){
		return (false);
	}
	heap_metadata = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_metadata) {
		return(false);
	}
	printf("Base data addr : %p\n", heap_data);
	t_chunk *first_chunk = new_chunk(heap_data, PAGE_SIZE, FREE);
	meta.first_chunk = first_chunk;
	meta.pages = 1;
	data.pages = 1;
	return (true);
}

//New chunk -> byte_before, block_addr, size

t_chunk	*new_chunk(void *data_addr, size_t size, t_chunk_state state) {
	//We have to check if there is enough space, otherwise we have to ask for a new page

	//Add a new chunk after the last chunk
	t_chunk *new = heap_metadata + (sizeof(t_chunk) * (meta.chunks_nb));
	new->data_addr = data_addr;
	new->size = size;
	new->state = state;
	new->next = NULL;
	new->prev = NULL;
	meta.last_chunk = new;
	meta.chunks_nb += 1;
	return (new);
}

/*
void add_next_chunk(t_chunk *chunk_before, t_chunk *chunk_to_add) {
	chunk_before->next = chunk_to_add;
	chunk_to_add->prev = chunk_before;
	if (chunk_before == meta.last_chunk) {
		meta.last_chunk = chunk_to_add;
	}
	meta.chunks_nb += 1;
}
void	*create_chunk(size_t size, t_chunk *chunk) {

	void *new_data_chunk = heap_data;
//	printf("Previously Allocated : %ld nb : %ld\n", previously_allocated, meta.chunks_nb - 1);
	if (chunk)
	{
		new_data_chunk = chunk->data_addr;
	}

	t_chunk *new_chunk = heap_metadata + sizeof(t_chunk) * (meta.chunks_nb - 1);
	t_chunk *next_free_chunk = heap_metadata + sizeof(t_chunk) * meta.chunks_nb;


	//fill_chunk(new_data_chunk, size, BUSY, new_chunk);
	size_t free_space = PAGE_SIZE * data.pages - (new_data_chunk - size - heap_data);
	//fill_chunk(new_data_chunk + size + canary size , free_space, FREE, next_free_chunk);

	add_next_chunk(new_chunk, next_free_chunk);

	printf("ETA %ld\n", free_space);

//	printf("last block %p\n", meta.last_block);

	return (new_data_chunk);
}*/

bool ask_new_data_page(size_t size) {
	printf("New page requested\n");
	void *new_data = mremap(heap_data, PAGE_SIZE * data.pages, PAGE_SIZE * data.pages + size, MREMAP_MAYMOVE);

	if (!new_data || new_data != heap_data) {
		fprintf(stderr, "mremap failed: %s, %ld\n", strerror(errno), PAGE_SIZE * data.pages + size);
		return (false);
	}
	heap_data = new_data;
	printf("%p, size %ld, \n", heap_data, PAGE_SIZE * data.pages + size);
	t_chunk *last_chunk = meta.last_chunk;
	last_chunk->size = last_chunk->size + (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) :(((size_t) (size / 4096) + 1) * PAGE_SIZE));
	data.pages += (size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1)));
	return (true);
}

void	*get_free_chunk(size_t size) {
	if (meta.first_chunk == meta.last_chunk && meta.first_chunk->state == FREE && meta.first_chunk->size >= size) {
		return (meta.first_chunk);
	}
	
	//else check if we have a free chunk that could contains the new requested data
	t_chunk *ptr = meta.first_chunk;

	if (ptr->state == FREE && ptr->size >= size) 
		return (ptr);

	while (ptr) {
		//	printf("we are in the loop: state %d size %ld block: %p\n", ptr->state, ptr->size, ptr);
		if (ptr->state == FREE && ptr->size >= size)
			return (ptr);
		printf("size %ld, size_max %ld, data: %ld\n", size, PAGE_SIZE * data.pages - (ptr->data_addr - heap_data), data.pages);
		if (size > PAGE_SIZE * data.pages - (ptr->data_addr - heap_data)) {
			if (!ask_new_data_page(size)) {
				printf("failed request new page\n");
				return (NULL);
			}
			ptr = meta.last_chunk;
			continue ;
		}
		ptr = ptr->next;
	}
	//printf("STATE %d Chunks %ld\n", ptr->state, meta.chunks_nb);
	return (NULL);
}

//This add split a data_chunk into 2 chunk, 1 BUSY and 1 FREE
void	split_data_chunk(t_chunk *chunk, size_t initial_data_size) {
	t_chunk *_new_chunk;

	if (chunk->size == initial_data_size)
		return ;
	printf("base size : %ld, chunk_size : %ld\n", initial_data_size, chunk->size);
	_new_chunk = new_chunk(chunk->data_addr + chunk->size, initial_data_size - chunk->size, FREE);
	_new_chunk->prev = chunk;
	if (chunk->next) {
		_new_chunk->next = chunk->next;
		if (chunk->next->next)
			chunk->next->next->prev = _new_chunk;
	}
	chunk->next = _new_chunk;
}

void *insert_chunk(t_chunk *free_chunk, size_t size) {
	size_t	initial_data_size = free_chunk->size;
	printf("size %ld address %p\n", initial_data_size, free_chunk->data_addr);
	free_chunk->state = BUSY;
	free_chunk->size = size;
	split_data_chunk(free_chunk, initial_data_size);
	printf("chunk info : %ld %d new chunk : %ld %d\n", free_chunk->size, free_chunk->state, free_chunk->next->size, free_chunk->next->state);
	return (free_chunk->data_addr);
}

void    *my_malloc(size_t size)
{
	void *res = NULL;
	if (!heap_data) {
		if (!init_heap()) {
			return (res);
		}
	}
	t_chunk *free_chunk = get_free_chunk(size);
	if (!free_chunk) {
		return (res);
	}
	res = insert_chunk(free_chunk, size);
	printf("res : %p\n", res);
	return (res);
}
void *get_chunk(void *addr) {
	for (size_t i = 0; i < meta.chunks_nb; i++) {
		t_chunk *chunk = meta.first_chunk + i;
		if (chunk->data_addr == addr) {
			return (chunk);
		}
	}
	return (NULL);
}

//function reset a chunk

void debug() {
	t_chunk *buff = meta.first_chunk;
	printf("DEBUG\n");
	while (buff) {
		printf("addr: %p size : %ld state : %d \n", buff->data_addr, buff->size, buff->state);
		buff = buff->next;
	}
}

void merge_chunk(t_chunk *chunk){
	if (!chunk->next)
		return ;
	t_chunk *next = chunk->next;
	t_chunk *prev = chunk->prev;
	if (next->state == FREE) {
		chunk->size += chunk->next->size;
		if (chunk->next->next)
		{
			chunk->next->next->prev = chunk;
			chunk->next = chunk->next->next;
		}
		else
		{
			meta.last_chunk = chunk;
			chunk->next = NULL;
		}
		memset(next, 0, sizeof(t_chunk));
	}
	if (prev && prev->state == FREE) {
		prev->size += chunk->size;
		if (chunk->next) {
			chunk->next->prev = prev;
			prev->next = chunk->next;
		} else {
			prev->next = NULL;
			meta.last_chunk = prev;
		}
		memset(chunk, 0, sizeof(t_chunk));
	}
}

/*
void optimize_memory(t_chunk *chunk_free) {

	if (meta.first_chunk != chunk_free && (chunk_free - 1)->state == FREE) {
		merge_chunk(chunk_free - 1, chunk_free);
		return ;
	}
	if (meta.last_chunk != chunk_free && (chunk_free + 1)->state == FREE) {
		merge_chunk(chunk_free, chunk_free + 1);
		return ;
	}
}*/

void    my_free(void *ptr)
{
	//Search for the ptr in the meta
	//Maybe check on a au moins 1 chunk ?
	if (!ptr) {
		return ;
	}
	t_chunk *chunk = get_chunk(ptr);
	if (!chunk) {
		return ;
	}
	chunk->state = FREE;
	merge_chunk(chunk);
	debug();
	//	optimize_memory();
	//optimize memory -> assembler les block free qui se suivent et unmap les page non utilise
}
void    *my_calloc(size_t nmemb, size_t size)
{
    (void) nmemb;
    (void) size;
    return NULL;
}

void    *my_realloc(void *ptr, size_t size)
{
    (void) ptr;
    (void) size;
    return NULL;
}

#ifdef DYNAMIC
void    *malloc(size_t size)
{
    return my_malloc(size);
}
void    free(void *ptr)
{
    my_free(ptr);
}
void    *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

void    *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);

}

#endif
