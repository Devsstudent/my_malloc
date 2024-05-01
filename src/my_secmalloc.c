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
	t_chunk *first_chunk = heap_metadata;
	fill_chunk(0, PAGE_SIZE, FREE, first_chunk);
	meta.chunks_nb = 1;
	meta.first_chunk = first_chunk;
	meta.last_chunk = first_chunk;
	meta.pages = 1;
	data.pages = 1;
	return (true);
}

//New chunk -> byte_before, block_addr, size

void	fill_chunk(size_t byte_before, size_t size, t_chunk_state state, t_chunk *new) {
	new->bytes_before = byte_before;
	new->data_addr = heap_data + byte_before;
	new->size = size;
	new->state = state;
	new->next = NULL;
	new->prev = NULL;
}

void add_next_chunk(t_chunk *chunk_before, t_chunk *chunk_to_add) {
	chunk_before->next = chunk_to_add;
	chunk_to_add->prev = chunk_before;
	if (chunk_before == meta.last_chunk) {
		meta.last_chunk = chunk_to_add;
	}
	meta.chunks_nb += 1;
}

void	*create_chunk(size_t size, size_t previously_allocated) {

//	printf("Previously Allocated : %ld nb : %ld\n", previously_allocated, meta.chunks_nb - 1);
	void *new_data_chunk = heap_data + previously_allocated;

	t_chunk *new_chunk = heap_metadata + sizeof(t_chunk) * (meta.chunks_nb - 1);
	t_chunk *next_free_chunk = heap_metadata + sizeof(t_chunk) * meta.chunks_nb;


	fill_chunk(previously_allocated, size, BUSY, new_chunk);
	size_t free_space = PAGE_SIZE * data.pages - (previously_allocated + size);
	fill_chunk(previously_allocated + size, free_space, FREE, next_free_chunk);

	add_next_chunk(new_chunk, next_free_chunk);

	printf("ETA %ld\n", PAGE_SIZE * data.pages - (previously_allocated + size));

//	printf("last block %p\n", meta.last_block);

	return (new_data_chunk);
}

void	*ask_new_data_page(size_t size) {

	void *new_data = mremap(heap_data, PAGE_SIZE * data.pages, PAGE_SIZE * data.pages + size, MREMAP_MAYMOVE);

	if (!new_data || new_data != heap_data) {
		fprintf(stderr, "mremap failed: %s, %ld\n", strerror(errno), PAGE_SIZE * data.pages + size);
		return (NULL);
	}
	heap_data = new_data;
	printf("%p, size %ld, \n", heap_data, PAGE_SIZE * data.pages + size);

	t_chunk *last_block = meta.last_chunk;
	last_block->size = last_block->size + size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) :(((size_t) (size / 4096) + 1) * PAGE_SIZE);
	data.pages += last_block->size + size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1));
	return (heap_data);
}

void	*get_free_chunk(size_t size) {
	size_t previous_allocated_size = 0;
	if (!heap_data) {
		if (!init_heap()) {
			return (NULL);
		}
	}
	//check if we have a free chunk that could contains the new requested data
	t_chunk *ptr = meta.first_chunk;
	//printf("STATE %d Chunks %ld\n", ptr->state, meta.chunks_nb);

	//First Block Dispo ?
	if (ptr->state == FREE && ptr->size >= size) {
		return (create_chunk(size, previous_allocated_size));
	}
	//loop pour verifier le prochain block dispo
	while (ptr != meta.last_chunk) {
	//	printf("we are in the loop: state %d size %ld block: %p\n", ptr->state, ptr->size, ptr);

		if (ptr->state == FREE && ptr->size >= size) {
			return (create_chunk(size, previous_allocated_size));
		}

		previous_allocated_size += ptr->size;

		if (size > PAGE_SIZE * data.pages - previous_allocated_size) {
			ask_new_data_page(size);
		}
		ptr = ptr + 1;
	}
	if (ptr->state == FREE && ptr->size >= size) {
		return (create_chunk(size, previous_allocated_size));
	}
	return (NULL);
}

void    *my_malloc(size_t size)
{
	return (get_free_chunk(size));
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

void merge_chunk(t_chunk *first_chunk, t_chunk *second_chunk) {
	first_chunk->size += second_chunk->size;
	memset(second_chunk, 0, sizeof(t_chunk));
}

void optimize_memory(t_chunk *chunk_free) {

	if (meta.first_chunk != chunk_free && (chunk_free - 1)->state == FREE) {
		merge_chunk(chunk_free - 1, chunk_free);
		return ;
	}
	if (meta.last_chunk != chunk_free && (chunk_free + 1)->state == FREE) {
		merge_chunk(chunk_free, chunk_free + 1);
		return ;
	}
}

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
