#define _GNU_SOURCE
#include <dlfcn.h> 
#include "my_secmalloc.private.h"

void			*heap_data = NULL;
void			*heap_meta = NULL;
t_info_malloc	info;

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i] != '\0')
	{
		if (write(fd, &(s[i]), sizeof(*s)))
			i++;
	}
}

static void	ft_putchar(char c, int fd)
{
	if (write(fd, &c, sizeof(c)))
		return ;
}

void	ft_putnbr_fd(int n, int fd)
{
	long	temp;

	temp = (long) n;
	if (n < 0)
	{
		ft_putchar('-', fd);
		temp *= -1;
	}
	if (temp < 10)
	{
		ft_putchar(temp + 48, fd);
		return ;
	}
	ft_putnbr_fd(temp / 10, fd);
	ft_putchar((temp % 10) + 48, fd);
}

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
//	printf("%i\n", PAGE_SIZE);
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
	t_chunk *first = new_chunk(heap_data, PAGE_SIZE, FREE);
	info.first_chunk = first;
	info.last_chunk = first;
}

size_t	get_meta_page_left() {
	size_t	used = info.total_meta_bytes;
	size_t	total_space = PAGE_SIZE * info.nb_meta_pages;
	return (total_space - used);
}

bool	new_meta_page() {
	void *heap_meta_remaped = mremap(heap_meta, PAGE_SIZE * info.nb_meta_pages, PAGE_SIZE * info.nb_meta_pages + PAGE_SIZE, MREMAP_MAYMOVE);
	if (!heap_meta_remaped) {
		printf("mmremap failed\n");
		return (false);
	}
	if (heap_meta_remaped != heap_meta) {
		printf("mmremap moved the base meta address\n");
		return (false);
	}
	info.nb_meta_pages += 1;
//	printf("meta size total %ld\n",PAGE_SIZE * info.nb_meta_pages);
	return (true);
}


//Peut etre je vais pouvoir remove le state car j'ajoute que des block free
//MetaInformationOnly
t_chunk *new_chunk(void *data_addr, size_t size, t_chunk_state state) {
	if (!data_addr) {
		return (NULL);
	}
	//A voir si pas <=
	//printf("%ld %ld\n", get_meta_page_left(), sizeof(t_chunk));
	if (get_meta_page_left() < sizeof(t_chunk)) {
		new_meta_page();
	}
	//printf("nb chunk %i addr %p\n", info.nb_chunks, heap_meta + (sizeof(t_chunk) * info.nb_chunks));
	//printf("%ld\n", sizeof(t_chunk) * info.nb_chunks);
	t_chunk *new = heap_meta + (sizeof(t_chunk) * (info.nb_chunks));
	new->data_addr = data_addr;
	new->state = state;
	//printf("size %ld\n", size);
	new->size = size;
	new->next = NULL;
	new->prev = info.last_chunk;
	info.nb_chunks += 1;
	info.total_meta_bytes += sizeof(t_chunk);
	t_chunk *last = info.last_chunk;
	if (last) {
		 last->next = new;
	}
	info.last_chunk = new;
	return (new);
}

bool	chunk_with_enough_space(size_t size, t_chunk **chunk_addr) {
	t_chunk *buff = info.first_chunk;

	while (buff) {
		if (buff->size >= size && buff->state == FREE) {
			*chunk_addr = buff;
			return (true);
		}
		buff = buff->next;
	}
	return (false);
}

bool	insert_into_chunk(t_chunk *chunk, size_t size) {

	t_chunk *next_chunk = chunk->next;
	size_t	free_bytes = chunk->size - size;
	t_chunk *new_chunk_free = new_chunk(chunk->data_addr + size, free_bytes, FREE);
//	printf("%ld chunksize %ld size %ld\n", free_bytes, chunk->size, size);
	if (!new_chunk_free) {
		return (false);
	}
	chunk->state = BUSY;
	uint64_t	canary = canary_random();
	chunk->canary = canary;
	(*((uint64_t *)(chunk->data_addr + size - sizeof(uint64_t)))) = canary;
	chunk->next = new_chunk_free;
	chunk->size = size;
	info.total_data_bytes += size;
	new_chunk_free->prev = chunk;
	if (next_chunk) {
		new_chunk_free->next = next_chunk;
		next_chunk->prev = new_chunk_free;
	}
	
	return (true);
}

bool	new_data_pages(size_t size) {
	void *heap_data_remaped = mremap(heap_data, PAGE_SIZE * info.nb_data_pages, PAGE_SIZE * info.nb_data_pages + size, MREMAP_MAYMOVE);
	if (!heap_data_remaped) {
		printf("mmremap failed\n");
		exit(1);
		return (false);
	}
	if (heap_data_remaped != heap_data) {
		printf("mmreamp moved the base addr\n");
		exit(1);
		return (false);
	}
	t_chunk *last_chunk = info.last_chunk;
	if (last_chunk->state == FREE) {
		last_chunk->size = last_chunk->size + (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) : (((size_t) (size / 4096) + 1) * PAGE_SIZE));
	} else {
		if (!new_chunk(heap_data, (size % 4096 == 0 ? (size / 4096 * PAGE_SIZE) : (((size_t) (size / 4096) + 1) * PAGE_SIZE)), FREE)) {
			return (false);
		}
	}
	size_t	tmp_nb = info.nb_data_pages;
	info.nb_data_pages += (size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1)));
	ft_putstr_fd("new data pages request\n", size);
	return (true);
}

void    *my_malloc(size_t size) {

	if (size == 0) {
		return (NULL);
	}

	if (!heap_data) {
		setup_heap();
	}

	//Get le previous chunk pour avoir la size du chunk
	t_chunk *chunk_addr = NULL;
	if (!chunk_with_enough_space(size + sizeof(uint64_t), &chunk_addr)) {
		if (!new_data_pages(size)) {
			return (NULL);
		}
		chunk_addr = info.last_chunk;
	}
	if (!insert_into_chunk(chunk_addr, size + sizeof(uint64_t))) {
		return 0;
	}
	ft_putnbr_fd(info.total_data_bytes, 2);
	write(2, "\n", 1);
	return chunk_addr->data_addr;
}

void *get_chunk(void *addr) {
	for (size_t i = 0; i < info.nb_chunks; i++) {
		t_chunk *chunk = info.first_chunk + i;
		if (chunk->data_addr == addr) {
			return (chunk);
		}
	}
	return (NULL);
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

	while (buff) {
		if (buff->state == FREE) {
			bytes += (size_t) buff->size;
		}
		buff = buff->next;
	}
	return (bytes);
}


void    my_free(void *ptr) {
	if (!ptr) {
		return ;
	}
	write(2, "free\n", 5);
	t_chunk *chunk = get_chunk(ptr);
	if (!chunk) {
		//error message
		return ;
	}
	if (chunk->state == FREE) {
		//Error message
		return ;
	}
	if (*(uint64_t *)(chunk->data_addr + chunk->size - sizeof(uint64_t)) != chunk->canary) {
		printf("EROORORRRORORO de CANNANANARY\n");
		//addLog("Trying to free :%p\nError: Canary failed\n", ptr);
		return ;
	}
	info.total_data_bytes -= chunk->size;
	chunk->state = FREE;
	merge_chunk(chunk);
//	memset(chunk->data_addr, 0, chunk->size);
//	size_t	free_space = free_data_in_bytes();
}

void    *my_calloc(size_t nmemb, size_t size) {

}


void    *my_realloc(void *ptr, size_t size) {

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
