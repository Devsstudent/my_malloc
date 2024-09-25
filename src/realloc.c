#include "malloc.h"

bool	realloc_ptr(t_chunk *ptr_chunk, size_t size, void **res,t_mem_zone **ptr_mem_zone) {
	bool	state;
	size_t	available_size;
	void	*ptr;
	t_chunk *next;

	state = false;
	ptr = (void *)ptr_chunk + sizeof(t_chunk);

	available_size = get_available_size(ptr_chunk->next, ptr_chunk->size);

//	if (available_size < size && available_size != ptr_chunk->size) {
		ft_printf("case maloc ++\n size %i\n", ptr_chunk->size);
		*res = ft_malloc(size);
		if (res)
		{
			state = true;
			ft_memcpy(*res, ptr, ptr_chunk->size);
			ft_free(ptr);
		}
		ft_printf("ALORS COMME CA MALLOC IL CRASH, %i\n", size);
/* { else {
		t_chunk *base_addr = NULL;
		//Recode pour uiliser le merge chunk
		//Puis split
		//Ne pas oublier de copier la data
		if (ptr_chunk->next && ptr_chunk->next->state == FREE) {
			ptr_chunk->state = FREE;
			(*ptr_mem_zone)->free_chunks += 1;
			(*ptr_mem_zone)->busy_chunks -= 1;
			merge_with_next(&ptr_chunk, *ptr_mem_zone, &base_addr);
		}
		split_chunk(ptr_chunk, *ptr_mem_zone, size);
		state = true;
		*res = ptr;
	}*/
	ft_printf("state %i\n", state);
	return (state);
}

void	*ft_realloc(void *ptr, size_t size) {
	void *res = NULL;
	t_mem_zone	*ptr_mem_zone = NULL;
	t_chunk		*ptr_chunk = NULL;

	size = (size + 31) & ~31;
	if (!ptr) {
		res = ft_malloc(size);
	} else if (ptr && size == 0) {
		ft_free(ptr);
	} else if (valid_ptr(&ptr_mem_zone, &ptr_chunk, ptr)) {
			ft_printf("realloc ptr valid\n");
//			pthread_mutex_lock(&mutex_malloc);
			if (ptr_chunk->state == FREE) {
				res = ptr;
			} else if (!realloc_ptr(ptr_chunk, size, &res, &ptr_mem_zone)){
				res = NULL;
			}
//			pthread_mutex_unlock(&mutex_malloc);
	} else {
		res = ft_malloc(size);
		if (res) {
			ft_free(ptr);
		}
	}
	return (res);
}
