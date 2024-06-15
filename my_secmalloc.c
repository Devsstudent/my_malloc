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

/*
bool	insert_into_chunk(t_chunk *chunk, size_t size) {

	t_chunk *next_chunk = chunk->next;
	size_t	free_bytes = chunk->size - size;
	//printf("freee %ld\n", free_bytes);
	size_t	aligned_forced_bytes = (size_t) (chunk->data_addr + size) % 8;
	//if (free_bytes - aligned_forced_bytes <= 0) {
		
//	}
	t_chunk *new_chunk_free = new_chunk(chunk->data_addr + size + aligned_forced_bytes, free_bytes - aligned_forced_bytes, FREE);
//	printf("%ld chunksize %ld size %ld\n", free_bytes, chunk->size, size);
	if (!new_chunk_free) {
		return (false);
	}
	chunk->state = BUSY;
	uint64_t canary = canary_random();
	chunk->canary = canary;
	//printf("%ld %ld\n", (size_t) size, info.total_data_bytes);
	(*((uint64_t *)(chunk->data_addr + size - sizeof(uint64_t)))) = canary;
	chunk->next = new_chunk_free;
	chunk->size = size;
	info.total_data_bytes += size + aligned_forced_bytes;
	new_chunk_free->prev = chunk;
	if (next_chunk) {
		new_chunk_free->next = next_chunk;
		next_chunk->prev = new_chunk_free;
	}
	
	return (true);
}*/

bool	new_data_pages(size_t size) {
	void *heap_data_remaped = mremap(heap_data, PAGE_SIZE * info.nb_data_pages, PAGE_SIZE * info.nb_data_pages + size, MREMAP_MAYMOVE);
	if (!heap_data_remaped) {
		//printf("mmremap failed\n");
		exit(1);
		return (false);
	}
	if (heap_data_remaped != heap_data) {
		//printf("mmreamp moved the base addr\n");
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
	info.nb_data_pages += (size % 4096 == 0 ? (size / 4096) : (((size_t) (size / 4096) + 1)));
	ft_putstr_fd("new data pages request\n", size);
	return (true);
}
/*
bool	chunk_with_enough_space(size_t size, t_chunk **chunk_addr) {
	t_chunk *buff = info.first_chunk;

	while (buff && buff != info.last_chunk) {
		//ft_putchar('\n', 2);
		//ft_putnbr_fd(size, 2);
		if (buff->size >= size && buff->state == FREE && buff->size) {
			*chunk_addr = buff;
			return (true);
		}
		buff = buff->next;
	}
	size_t	aligned_needed = (size_t) (buff->data_addr + size) % 8;
	if (buff->size >= size + aligned_needed && buff->state == FREE) {
		*chunk_addr = buff;
		return (true);
	}
	return (false);
}*/

//Peut etre je vais pouvoir remove le state car j'ajoute que des block free
//MetaInformationOnly
//
//
t_chunk *new_chunk(void *data_addr, size_t size, t_chunk_state state) {
//We could check if there is a chunk free, to setup the chunk instead of allocating a new one
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
	t_chunk *new = heap_meta + (sizeof(t_chunk) * (info.len_meta_list));
	new->data_addr = data_addr;
	new->state = state;
	//printf("size %ld\n", size);
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

	if (size == 0) {
		return (NULL);
	}

	if (!heap_data) {
		setup_heap();
	}
	ft_putstr_fd("malloc : ", 2);
	ft_putnbr_fd(size, 2);
	ft_putstr_fd("\n", 2);

	//Get le previous chunk pour avoir la size du chunk
	t_chunk *chunk_addr = NULL;
	//printf("last : %ld %p\n", info.last_chunk->size, info.last_chunk);
//	write(2, "ici:", 4);
//	ft_putnbr_fd(info.len_meta_list ,2);
//	ft_putchar('\n', 2);
	//printf("%ld %ld\n", size, info.total_meta_bytes);
	//ft_putnbr_fd(info.total_meta_bytes, 2);
	//if (!chunk_with_enough_space(size + sizeof(uint64_t), &chunk_addr)) {
	if (info.last_chunk->state != FREE) {
		write(2, "BUG\n", 4);
		exit(1);
	}
	size_t	aligned = (size_t) (info.last_chunk->data_addr + size + sizeof(uint64_t)) % 8;
	size_t	aligned_needed = aligned + size + sizeof(uint64_t);
		if (info.last_chunk->size < aligned_needed) {
			if (!new_data_pages(aligned_needed)) {
				return (NULL);
			}
		}
//		info.last_chunk->aligned = aligned;
		info.last_chunk->state = BUSY;
		info.last_chunk->size = aligned_needed;
		ft_putstr_fd("chunk size allocated: ", 2);
		ft_putnbr_fd(info.last_chunk->size, 2);
		ft_putchar(' ', 2);
		ft_putnbr_fd(aligned_needed, 2);
		ft_putchar('\n', 2);
		chunk_addr = info.last_chunk;
		//printf("base %p\n", chunk_addr->data_addr);
		size_t	size_new_chunk = info.last_chunk->size - aligned_needed;
		void *new_data_addr = info.last_chunk->data_addr + aligned_needed;
		info.last_chunk->canary =  canary_random();
		*(uint64_t*)(info.last_chunk->data_addr + aligned_needed - sizeof(uint64_t)) = info.last_chunk->canary;
		//printf("alloc ddr : %p, %ld canary addr%p\n", info.last_chunk->data_addr, aligned_needed, info.last_chunk->data_addr + aligned_needed - sizeof(uint64_t));
		t_chunk *new_last_chunk = new_chunk(new_data_addr, size_new_chunk, FREE);
		
		//split
	//if (!insert_into_chunk(chunk_addr, size + sizeof(uint64_t))) {
	//	return 0;
	//}
	//ft_putnbr_fd(info.total_data_bytes, 2);
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


//On a bien un bug ici sur le merging et le next, desfois on perd le NULL

//Tester si a 2 endroit, pas ala fin de la memeoire le merge_hcunk pour checker que la lendes block reste bien reduit bien de 1
void merge_chunk(t_chunk *chunk){
	t_chunk *next = chunk->next;
	t_chunk *prev = chunk->prev;
	ft_putnbr_fd(info.len_meta_list, 2);
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
		//ft_putnbr_fd(info.len_meta_list, 2);
		info.len_meta_list -= 1;
	//	write(2, "ahahN", 5);
		//ft_putnbr_fd(info.len_meta_list, 2);
		memset(next, 0, sizeof(t_chunk));
	}
	//ft_putnbr_fd(next->state, 2);
	if (prev && prev->state == FREE) {
		prev->size += chunk->size;
		if (chunk->next) {
			chunk->next->prev = prev;
			prev->next = chunk->next;
		} else {
			prev->next = NULL;
			info.last_chunk = prev;
		}
		info.len_meta_list -= 1;
	//	write(2, "ahahP", 4);
		memset(chunk, 0, sizeof(t_chunk));
	}
	ft_putstr_fd("and", 2);
	ft_putnbr_fd(info.len_meta_list, 2);
	//ft_putnbr_fd(info.len_meta_list, 2);
}

size_t	free_data_in_bytes() {
	t_chunk	*buff = info.first_chunk;
	size_t	bytes = 0;

	ft_putnbr_fd(info.len_meta_list, 2);
	int i = 0;
	while (info.last_chunk && buff && buff != info.last_chunk) {
		if (buff->state == FREE) {
//			ft_putstr_fd("chunk :", 2);
//			ft_putnbr_fd(buff->size, 2);
//			ft_putstr_fd("\n", 2);
			bytes += (size_t) buff->size;
		}
		buff = buff->next;
		i++;
	}
	if (i == info.len_meta_list) {
		return bytes;
	}
	if (buff->state == FREE) {
		bytes += (size_t) buff->size;
	}
	return (bytes);
}

void    my_free(void *ptr) {
	if (!ptr) {
		return ;
	}
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
		ft_putstr_fd("EROORORRRORORO de CANNANANARY\n", 2);
		//printf("%ld %p\n", chunk->size, chunk->data_addr);
		ft_putnbr_fd(chunk->size, 2);
		ft_putchar('\n', 2);
		//printf("ERROR CANARY %p\n", (chunk->data_addr + chunk->size - sizeof(uint64_t)));
		//addLog("Trying to free :%p\nError: Canary failed\n", ptr);
		return ;
	}
	ft_putstr_fd("free : ", 2);
	ft_putnbr_fd(chunk->size, 2);
	ft_putchar('\n', 2);
	info.total_data_bytes -= chunk->size;
	//ft_putnbr_fd(chunk->size, 2);
	//ft_putchar('\n', 2);
	chunk->state = FREE;
//	write(2, "free", 4);
	//merge_chunk(chunk);
//	memset(chunk->data_addr, 0, chunk->size);
	size_t	free_space = free_data_in_bytes();
	if (free_space < 0) {
		ft_putstr_fd("free :", 2);
		ft_putnbr_fd(free_space, 2);
		ft_putstr_fd("\n", 2);
	}
}

void    *my_calloc(size_t nmemb, size_t size) {
	write(2, "calloc\n", 7);
	void *res = my_malloc(nmemb * size);
	ft_putnbr_fd(nmemb * size, 2);
	if (!res) {
		return NULL;
	}
	memset(res, 0, nmemb * size);
	return (res);
}


void    *my_realloc(void *ptr, size_t size) {
	write(2, "realloc\n", 8);
	void *res = NULL;
	if (size == 0 && ptr) {
		my_free(ptr);
		return (res);
	}
	if (!ptr && size > 0) {
		res = my_malloc(size);
		ft_putnbr_fd(size, 2);
		if (res)
			return (res);
	}
	t_chunk *info_ptr = get_chunk(ptr);
	if (info_ptr) {
		res = my_malloc(size);
		memcpy(res, ptr, info_ptr->size - sizeof(uint64_t));
		my_free(ptr);
		return (res);
	}
	return NULL;
}

int main(void) {
	int i = 1;
	void *ptr;
	ptr = my_malloc(10);
	void *ptr2 = my_malloc(500);
	void *ptr3 = my_malloc(1000);
	my_free(ptr3);
	my_free(ptr);
	my_free(ptr2);
	i++;
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
