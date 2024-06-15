#define _GNU_SOURCE
#include <dlfcn.h> 
#include "my_secmalloc.private.h"

void			*heap_data = NULL;
void			*heap_meta = NULL;
t_info_malloc	info;

uint64_t canary_random() {
    uint64_t random_value;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open /dev/urandom");
        exit(EXIT_FAILURE);
    }

    ssize_t result = read(fd, &random_value, sizeof(random_value));
    if (result < 0) {
        perror("Failed to read from /dev/urandom");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return random_value;
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
		write(2, buffer, strlen(buffer));
		return ;
	}
	va_list args;
	va_start (args, format);
	if (vsnprintf (buffer, 256,format, args) < 0)
	{
		perror (buffer);
		write(2, buffer, strlen(buffer));
		va_end (args);
		close(fd);
		return ;
	}
	write(fd, buffer, strlen(buffer));
	va_end (args);
	close(fd);
}

bool	setup_heap() {
	heap_data = mmap((void *)(0x56128d551000), PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_data) {
		return (false);
	}
	heap_meta = mmap((void *)(4096 * 100000), PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!heap_meta) {
		return (false);
	}
	info.nb_data_pages = 1;
	info.nb_meta_pages = 1;
	t_chunk *first = new_chunk(heap_data, PAGE_SIZE);
	info.first_chunk = first;
	info.last_chunk = first;
	return (true);
}

size_t	get_meta_page_left() {
	size_t	used = info.total_meta_bytes;
	size_t	total_space = PAGE_SIZE * info.nb_meta_pages;
	return (total_space - used);
}

bool	new_meta_page() {
	addLog("New meta page requested\n");
	void *heap_meta_remaped = mremap(heap_meta, PAGE_SIZE * info.nb_meta_pages, PAGE_SIZE * info.nb_meta_pages + PAGE_SIZE, MREMAP_MAYMOVE);
	if (!heap_meta_remaped) {
		addLog("mmremap failed\n");
		return (false);
	}
	if (heap_meta_remaped != heap_meta) {
		addLog("mmremap moved the base meta address\n");
		return (false);
	}
	info.nb_meta_pages += 1;
	return (true);
}

bool	new_data_pages(size_t size) {
	addLog("New data pages requested\n");
	void *heap_data_remaped = mremap(heap_data, PAGE_SIZE * info.nb_data_pages, PAGE_SIZE * info.nb_data_pages + size, MREMAP_MAYMOVE);
	if (!heap_data_remaped) {
		addLog("mmremap failed\n");
		exit(1);
		return (false);
	}
	if (heap_data_remaped != heap_data) {
		addLog("mmremap moved the base addr\n");
		exit(1);
		return (false);
	}
	t_chunk *last_chunk = info.last_chunk;
	if (last_chunk->state == FREE) {
		last_chunk->size = last_chunk->size + (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) : (((size_t) (size / 4096) + 1) * PAGE_SIZE));
	} else {
		if (!new_chunk(heap_data, (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) : (((size_t) (size / 4096) + 1) * PAGE_SIZE)))) {
			return (false);
		}
	}
	info.nb_data_pages += (size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1)));
	return (true);
}

t_chunk *new_chunk(void *data_addr, size_t size) {
	if (!data_addr) {
		return (NULL);
	}
	if (get_meta_page_left() < sizeof(t_chunk)) {
		new_meta_page();
	}
	t_chunk *new = heap_meta + (sizeof(t_chunk) * (info.len_meta_list));
	new->data_addr = data_addr;
	new->state = FREE;
	new->size = size;
	new->next = NULL;
	new->prev = info.last_chunk;
	info.len_meta_list += 1;
	info.total_meta_bytes += sizeof(t_chunk);
	t_chunk *last = info.last_chunk;
	if (last) {
		 last->next = new;
	}
	info.last_chunk = new;
	return (new);
}

void    *my_malloc(size_t size) {
	addLog("\nAppel a Malloc for a size of %ld\n", size);
	if (size == 0) {
		return (NULL);
	}

	if (!heap_data) {
		setup_heap();
	}
	t_chunk *chunk_addr = NULL;
	size_t	aligned = (size_t) (info.last_chunk->data_addr + size + sizeof(uint64_t)) % 8;
	size_t	aligned_needed = aligned + size + sizeof(uint64_t);
	if (info.last_chunk->size < aligned_needed) {
		if (!new_data_pages(aligned_needed)) {
			return (NULL);
		}
	}
	size_t	size_new_chunk = info.last_chunk->size - aligned_needed;
	info.last_chunk->state = BUSY;
	info.last_chunk->size = aligned_needed;
	chunk_addr = info.last_chunk;
	void *new_data_addr = info.last_chunk->data_addr + aligned_needed;
	info.last_chunk->canary =  canary_random();
	*(uint64_t*)(info.last_chunk->data_addr + aligned_needed - sizeof(uint64_t)) = info.last_chunk->canary;
	new_chunk(new_data_addr, size_new_chunk);
	addLog("Appel a Malloc, return address : %p\n", chunk_addr->data_addr);
	return chunk_addr->data_addr;
}

void *get_chunk(void *addr) {
	for (size_t i = 0; i < info.len_meta_list; i++) {
		t_chunk *chunk = info.first_chunk + i;
		if (chunk->data_addr == addr) {
			return (chunk);
		}
	}
	return (NULL);
}

void merge_chunk(t_chunk *chunk){
	t_chunk *next = chunk->next;
	t_chunk *prev = chunk->prev;
	if (next && next->state == FREE) {
		chunk->size += chunk->next->size;
		if (chunk->next->next)
		{
			chunk->next->next->prev = chunk;
			chunk->next = chunk->next->next;
		}
		else
		{
			info.last_chunk = chunk;
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
			info.last_chunk = prev;
		}
		memset(chunk, 0, sizeof(t_chunk));
	}
}

size_t	free_data_in_bytes() {
	t_chunk	*buff = info.first_chunk;
	size_t	bytes = 0;

	size_t i = 0;
	while (buff) {

		if (buff->state == FREE) {
			bytes += (size_t) buff->size;
		}
		buff = buff->next;
		i++;
	}
	return (bytes);
}

void    my_free(void *ptr) {
	addLog("\nAppel a free on %p\n", ptr);
	if (!ptr) {
		return ;
	}
	t_chunk *chunk = get_chunk(ptr);
	if (!chunk) {
		addLog("free on a unknow address: %p\n", ptr);
		return ;
	}
	if (chunk->state == FREE) {
		addLog("double free for %p\n", ptr);
		return ;
	}
	if (*(uint64_t *)(chunk->data_addr + chunk->size - sizeof(uint64_t)) != chunk->canary) {
		addLog("Trying to free :%p\nError: Canary failed (Heap Overflow)\n", ptr);
		return ;
	}
	info.total_data_bytes -= chunk->size;
	chunk->state = FREE;
	merge_chunk(chunk);
}

void    *my_calloc(size_t nmemb, size_t size) {
	addLog("\nAppel a Calloc, for a total size of %ld\n", nmemb * size);
	void *res = my_malloc(nmemb * size);
	if (!res) {
		return NULL;
	}
	memset(res, 0, nmemb * size);
	addLog("Appel a Calloc, return %p\n", res);
	return (res);
}


void    *my_realloc(void *ptr, size_t size) {
	addLog("\nAppel a Realloc, for a total size of %ld\n", size);
	void *res = NULL;
	if (size == 0 && ptr) {
		my_free(ptr);
		return (res);
	}
	if (!ptr && size > 0) {
		res = my_malloc(size);
		if (res) {
			addLog("Appel a Realloc, return %p\n", res);
			return (res);
		}
	}
	t_chunk *info_ptr = get_chunk(ptr);
	if (info_ptr) {
		res = my_malloc(size);
		memcpy(res, ptr, info_ptr->size - sizeof(uint64_t));
		my_free(ptr);
		addLog("Appel a Realloc, return %p\n", res);
		return (res);
	}
	addLog("Appel a Realloc, return %p\n", res);
	return res;
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
