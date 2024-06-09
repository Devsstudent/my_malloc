#define _GNU_SOURCE
#include <dlfcn.h>
#include "my_secmalloc.private.h"
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

void	*heap_data;
void	*heap_metadata;
t_metadata	meta;
t_data		data;

//Know when the program end to make a report of the leaks 
//Find a way to get the adress of the heap
bool	init_heap(void) {

/*
	unsigned char vec;
	if (mincore((void *)(0xaaabb5479000), 4096, &vec) < 0) {
		//initialize metatadata struct
		//initialize data struct
		addLog("Already allocated Base Address\n");
	}*/
	heap_metadata = mmap((void *)(0x61f546481000), PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_metadata) {
		return(false);
	}
	heap_data = mmap((void *)(0x61f54648f000), PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_data){
		return (false);
	}
	meta.pages = 1;
	addLog("Base data addr : %p\n", heap_data);
	t_chunk *first_chunk = new_chunk(heap_data, PAGE_SIZE, FREE);
	addLog("first chunk : %p\n",first_chunk);
	if (!first_chunk) {
		return (false);
	}
	meta.first_chunk = first_chunk;
	data.pages = 1;
	return (true);
}

uint8_t	canary_random() {
	srand(time(NULL));
	return (uint8_t)rand();
}

bool ask_new_meta_page() {
	addLog("New page of meta data requested\n");
	void *new_meta = mremap(heap_metadata, PAGE_SIZE * meta.pages, PAGE_SIZE * meta.pages + PAGE_SIZE - 1, MREMAP_MAYMOVE);

	if (!new_meta || new_meta != heap_metadata) {
		addLog("Bruh %p %p\n", new_meta, heap_metadata);
		addLog("mremap failed: %s, %ld\n", strerror(errno), PAGE_SIZE * meta.pages + 1);
		return (false);
	}
	heap_metadata = new_meta;
	printf("META PAGE : %p, size %ld, \n", heap_metadata, PAGE_SIZE * meta.pages + 1);
	meta.pages += 1;
	//new free chunk
//	t_chunk *tmp = meta.last_chunk;

//	size_t chunkbypage = PAGE_SIZE / sizeof(t_chunk);
//	meta.last_chunk = heap_metadata + sizeof(t_chunk) * (meta.pages - 1) * chunkbypage;
//	meta.last_chunk->data_addr = tmp->data_addr;
//	meta.last_chunk->state = tmp->state;
//	meta.last_chunk->size = tmp->size;
//	meta.last_chunk->canary = tmp->canary;
//	meta.last_chunk.next = tmp.next
//	Rever le next et le prev
//	tmp->next = meta.last_chunk;
//	tmp->size = 0;
//	meta.last_chunk->prev = tmp;
//	addLog("ETA %p\n", meta.last_chunk);
	meta.chunks_nb += 1;
	return (true);
}

t_chunk	*new_chunk(void *data_addr, size_t size, t_chunk_state state) {
	//We have to check if there is enough space, otherwise we have to ask for a new page

	//Add a new chunk after the last chunk
	addLog("New chunk %d\n", size);
	addLog("NEW META PAGE ? %ld, %ld\n", meta.chunks_nb * sizeof(t_chunk) + sizeof(t_chunk), meta.pages * PAGE_SIZE);
	if (meta.chunks_nb * sizeof(t_chunk) + sizeof(t_chunk) > meta.pages * PAGE_SIZE) {
		if (!ask_new_meta_page()) {
			return (NULL);
		}
	}
	addLog("ETA1 \n");
	t_chunk *new = heap_metadata + (sizeof(t_chunk) * (meta.chunks_nb));
	addLog("ETA2 %p %i\n", new, state);
	if (state == BUSY) {
		uint8_t	canary = canary_random();
		new->canary = canary;
		printf("CANANRY : %d\n", canary);
		*(uint8_t *)(data_addr + (size - 1)) = canary;
	}
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
	addLog("New page of data requested\n");

	//with thoses flags, we are getting anoter address, it seems, i have to compare head_data valuue and new_data
	void *new_data = mremap(heap_data, PAGE_SIZE * data.pages, PAGE_SIZE * data.pages + size, MREMAP_MAYMOVE, heap_data);

	if (!new_data || new_data != heap_data) {
		fprintf(stderr, "mremap failed: %s, %ld\n", strerror(errno), PAGE_SIZE * data.pages + size);
		return (false);
	}
	heap_data = new_data;
	printf("DATA PAGE : %p, size %ld, \n", heap_data, PAGE_SIZE * data.pages + size);
	t_chunk *last_chunk = meta.last_chunk;
	last_chunk->size = last_chunk->size + (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) :(((size_t) (size / 4096) + 1) * PAGE_SIZE));
	data.pages += (size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1)));
	return (true);
}

void	*get_free_chunk(size_t size) {
//	addLog("Looking for chunk, %p\n", meta.first_chunk);
	if (meta.first_chunk == meta.last_chunk && meta.first_chunk->state == FREE && meta.first_chunk->size >= size) {
		addLog("Looking for chunk, result in :%p\n", meta.first_chunk);
		return (meta.first_chunk);
	}
	
	//else check if we have a free chunk that could contains the new requested data
	t_chunk *ptr = meta.first_chunk;

	addLog("Looking for chunk 2: %ld ptr size %ld ptr state %d, test %d\n", size, ptr->size, ptr->state, (ptr->state == FREE && ptr->size >= size));
	if (ptr->state == FREE && ptr->size >= size)  {
		addLog("ptdr %p\n", ptr);
		return (ptr);
	}
	addLog("POPNERT %ld %ld\n", data.total + size, PAGE_SIZE * data.pages);
	if (data.total + size > PAGE_SIZE * data.pages) {
		if (!ask_new_data_page(size)) {
			addLog("failed request new page\n");
			return (NULL);
		}
		ptr = meta.last_chunk;

	}

	while (ptr) {
	//	addLog("we are in the loop: state %d size %ld block: %p\n", ptr->state, ptr->size, ptr);
		if (ptr->state == FREE && ptr->size >= size)
			return (ptr);
	//	addLog("size %ld, size_max %ld, data: %ld\n", size, PAGE_SIZE * data.pages - (ptr->data_addr - heap_data), data.pages);
//		addLog("size %ld, page %ld\n", size, PAGE_SIZE * data.pages - (ptr->data_addr - heap_data));
		ptr = ptr->next;
	}
	//printf("STATE %d Chunks %ld\n", ptr->state, meta.chunks_nb);
	return (NULL);
}

//This add split a data_chunk into 2 chunk, 1 BUSY and 1 FREE
bool	split_data_chunk(t_chunk *chunk, size_t initial_data_size) {
	t_chunk *_new_chunk;

	if (chunk->size == initial_data_size)
		return true;
	addLog("base size : %ld, chunk_size : %ld, metasize %ld\n", initial_data_size, chunk->size, meta.pages * PAGE_SIZE);
	_new_chunk = new_chunk(chunk->data_addr + chunk->size, initial_data_size - chunk->size, FREE);
	addLog("after \n");
	if (!_new_chunk) {
		return (false);
	}
	_new_chunk->prev = chunk;
	if (chunk->next) {
		_new_chunk->next = chunk->next;
		if (chunk->next->next)
			chunk->next->next->prev = _new_chunk;
	}
	chunk->next = _new_chunk;
	return (true);
}

void *insert_chunk(t_chunk *free_chunk, size_t size) {
	size_t	initial_data_size = free_chunk->size;
	addLog("size %ld address %p\n", initial_data_size, free_chunk->data_addr);
	free_chunk->state = BUSY;
	free_chunk->size = size;
	uint8_t	canary = canary_random();
	free_chunk->canary = canary;
//	printf("CANANRY : %d\n", canary);
	*(uint8_t *)(free_chunk->data_addr + (size - 1)) = canary;
	if (!split_data_chunk(free_chunk, initial_data_size)) {
		return (NULL);
	}
	//printf("chunk info : %ld %d new chunk : %ld %d\n", free_chunk->size, free_chunk->state, free_chunk->next->size, free_chunk->next->state);
	return (free_chunk->data_addr);
}

void debug() {
	t_chunk *buff = meta.first_chunk;
//	printf("DEBUG\n");
	size_t	size = 0;
	while (buff) {
//		printf("addr: %p size : %ld state : %d \n", buff->data_addr, buff->size, buff->state);
		if (buff->state == BUSY) {
			size += buff->size;
		}
		buff = buff->next;
	}
	addLog("Used : %ld\n", size);
}

void    *my_malloc(size_t size)
{
	//addLog("Appel a malloc, for block of size %ld\n", size);
	//addLog("PID: %ld, heap_data %p\n", getpid(), heap_data);
	//addLog("full size : %ld\n", data.pages * PAGE_SIZE);
	void *res = NULL;
	if (!heap_data) {
		if (!init_heap()) {
			addLog("Fail init heap\n");
			return (res);
		}
	}
	t_chunk *free_chunk = get_free_chunk(size + 1);
	addLog("free chunk :%p, PID: %ld\n", free_chunk, getpid());
	if (!free_chunk) {
		addLog("No free chunk\n");
		return (res);
	}
	res = insert_chunk(free_chunk, size + 1);
	addLog("Address allocated by malloc: %p, size : %d\n", res, size);
	addLog("full size meta : %ld used %ld\n", meta.pages * PAGE_SIZE, meta.chunks_nb * sizeof(t_chunk));
	addLog("Full size : %ld, used %ld\n", data.pages * PAGE_SIZE, data.total);
	debug();
	data.total += size + 1;
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

size_t	free_data_in_bytes() {
	t_chunk	*buff = meta.first_chunk;
	size_t	bytes = 0;

	while (buff) {
		if (buff->state == FREE) {
			bytes += (size_t) buff->size;
		}
		buff = buff->next;
	}
	return (bytes);
}

void	addLog(char *format, ...) {
	char *buffer = (char *)alloca(256);
	char *logFile = getenv("MSM_OUTPUT");

	if (!logFile) {
		return ;
	}
	int fd = open(logFile, O_CREAT | O_APPEND | O_RDWR, 0640);
	if (fd < 0) {
		perror (buffer);
	//	write(2, buffer, strlen(buffer));
		return ;
	}
	va_list args;
	va_start (args, format);
	if (vsnprintf (buffer, 256,format, args) < 0)
	{
		perror (buffer);
	//	write(2, buffer, strlen(buffer));
		va_end (args);
		return ;
	}
//	write(2, buffer, strlen(buffer));
	write(fd, buffer, strlen(buffer));
	va_end (args);
}

void    my_free(void *ptr)
{
	//Search for the ptr in the meta
	//Maybe check on a au moins 1 chunk ?
	addLog("Appel a Free, addr : %p\n", ptr);
	if (!ptr) {
		return ;
	}
	t_chunk *chunk = get_chunk(ptr);
	if (!chunk) {
		addLog("Trying to free :%p failed\nError: unallocated\n", ptr);
		return ;
	}
	if (chunk->state == FREE) {
		addLog("Trying to free :%p failed\nError: double free\n", ptr);
		return ;
	}
	data.total -= chunk->size;
	printf("%i, %i\n", *(uint8_t*)(chunk->data_addr + chunk->size - 1), chunk->canary);
	if (*(uint8_t *)(chunk->data_addr + chunk->size - 1) != chunk->canary) {
	//	printf("EROORORRRORORO de CANNANANARY\n");
		addLog("Trying to free :%p\nError: Canary failed\n", ptr);
		return ;
	}
	chunk->state = FREE;
	merge_chunk(chunk);
	addLog("total : %ld\n", free_data_in_bytes());
	size_t	free_space = free_data_in_bytes();
	if (free_space == data.pages * PAGE_SIZE) {
		//We free the data
		addLog("Unmap : %p %ld %ld\n", heap_data, free_space, data.pages * PAGE_SIZE);
		if (munmap(heap_data, data.pages * PAGE_SIZE) < 0 ) {
			return ;
		}
		if (munmap(heap_metadata, meta.pages * PAGE_SIZE) < 0 ) {
			return ;
		}
		heap_data = NULL;
		heap_metadata = NULL;
		data.pages = 0;
		meta.pages = 0;
		meta.first_chunk = NULL;
		meta.last_chunk = NULL;
	}
//	debug();
	//	optimize_memory();
	//optimize memory -> assembler les block free qui se suivent et unmap les page non utilise
}
void    *my_calloc(size_t nmemb, size_t size)
{
	addLog("Appel a Calloc, for a size of %ld\n", nmemb * size);
	void *res = my_malloc(nmemb * size);
	if (!res) {
		return (NULL);
	}
	memset(res, 0, nmemb * size);
	addLog("Retunr Address of calloc: %p, size : %d\n", res, size * nmemb);
	return (res);
}

void    *my_realloc(void *ptr, size_t size)
{
	addLog("Appel a realloc at addr : %p for a new size of %ld\n", ptr, size);
	void *res = NULL;
	if (size == 0 && ptr) {
		my_free(ptr);
		return (res);
	}
	if (!ptr && size > 0) {
		res = my_malloc(size);
		if (res) {
			return (res);
		}
	}
	t_chunk *info_ptr = get_chunk(ptr);
	if (info_ptr) {
		printf("info size %ld, size %ld\n", info_ptr->size, size);
		if (info_ptr->size >= size) {
			insert_chunk(info_ptr, size + 1);
			return (ptr);
		}
		else {
			res = my_malloc(size);
			if (!res) {
				return (ptr);
			}
			memcpy(res, ptr, info_ptr->size - 1);
			my_free(ptr);
			return (res);
		}
	}
	return (NULL);
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
