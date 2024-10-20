#include "ft_malloc.h"

bool	realloc_ptr(t_chunk *ptr_chunk, size_t size, void **res,t_mem_zone **ptr_mem_zone) {
	bool	state;
	size_t	available_size;
	void	*ptr;
	int		size_mem_cpy = 0;

	state = false;
	ptr = (void *)ptr_chunk + sizeof(t_chunk);

	available_size = get_available_size(ptr_chunk->next, ptr_chunk->size);

	if (available_size < size && available_size != ptr_chunk->size) {
		pthread_mutex_unlock(&mutex_malloc);
		*res = ft_malloc(size);
		if (ptr_chunk->size < size) {
			size_mem_cpy = ptr_chunk->size;
		} else {
			size_mem_cpy = size;
		}
		if (res)
		{
			state = true;
			ft_memcpy(*res, ptr, size_mem_cpy);
			ft_free(ptr);
		}
	} else {
		if (ptr_chunk->next && ptr_chunk->next->state == FREE) {
			ptr_chunk->state = FREE;
			(*ptr_mem_zone)->free_chunks += 1;
			if ((*ptr_mem_zone)->busy_chunks > 0) {
				(*ptr_mem_zone)->busy_chunks -= 1;
			}
			merge_with_next(&ptr_chunk, *ptr_mem_zone);
		}
		//Il faut tester quand on realloc un pointeur, en reduisant ca size, genre il faut split le next avec un free toussa
		//ft_printf("ALLOO SWTF\n");
		split_chunk(ptr_chunk, *ptr_mem_zone, size);
		state = true;
		*res = (void *)ptr_chunk + sizeof(t_chunk);
		//ft_printf("%p %p %p\n", ptr, *res, ptr_chunk);
		pthread_mutex_unlock(&mutex_malloc);
	}
	return (state);
}

void	*ft_realloc(void *ptr, size_t size) {
	void *res = NULL;
	t_mem_zone	*ptr_mem_zone = NULL;
	t_chunk		*ptr_chunk = NULL;
	bool		valid = false;

	size = (size + 15) & ~15;
	pthread_mutex_lock(&mutex_malloc);
	valid = valid_ptr(&ptr_mem_zone, &ptr_chunk, ptr);
	pthread_mutex_unlock(&mutex_malloc);
	if (!ptr) {
		res = ft_malloc(size);
	} else if (ptr && size == 0) {
		ft_free(ptr);
	} else if (!pthread_mutex_lock(&mutex_malloc) && valid) {
			if (ptr_chunk->state == FREE) {
				res = ptr;
				pthread_mutex_unlock(&mutex_malloc);
			} else if (!realloc_ptr(ptr_chunk, size, &res, &ptr_mem_zone)){
				res = NULL;
			}
	} else {

		pthread_mutex_unlock(&mutex_malloc);
		size_t usable_size = malloc_usable_size(ptr);
		res = ft_malloc(size);
		if (res) {
			ft_memcpy(res, ptr, usable_size);
			ft_free(ptr);
		}
	}
	//ft_printf("%p %i from realloc\n", res, (int)size);
	return (res);
}
