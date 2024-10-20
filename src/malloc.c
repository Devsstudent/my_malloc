/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_bis.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 22:04:15 by odessein          #+#    #+#             */
/*   Updated: 2024/09/25 16:31:17 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "ft_malloc.h"


t_alloc_info g_alloc_info = {
	.tiny = NULL,
	.last_tiny = NULL,
	.nb_tiny_elems = 0,
	.small = NULL,
	.last_small = NULL,
	.nb_small_elems = 0,
	.large = NULL,
	.last_large = NULL,
	.nb_large_elems = 0,
};

pthread_mutex_t mutex_malloc = PTHREAD_MUTEX_INITIALIZER;
t_alloc_info get_alloc_info() {
	return g_alloc_info;
}

void *ft_malloc(size_t size) {
	//ft_printf("Malloc %i\n", size);
	size = (size + 15) & ~15;

	pthread_mutex_lock(&mutex_malloc);
	t_mem_zone *current_zone = get_current_zone(size);

	if (!current_zone) {
			pthread_mutex_unlock(&mutex_malloc);
		//Error on a crash
		return NULL;
	}

	t_chunk *asked_chunk = get_chunk(current_zone, size);
	pthread_mutex_unlock(&mutex_malloc);
	if (!asked_chunk) {
		//Error on a pas trouver de chunk qui match (ca doit jamais arriver)
		return NULL;
	}
	//ft_printf("%p returned\n", (void *)(asked_chunk) + sizeof(t_chunk));
	return ((void *)(asked_chunk) + sizeof(t_chunk));
}


#ifdef DYNAMIC

//#warning "DYNAMIC is defined"
void    *malloc(size_t size)
{
	//ft_printf("Malloc %i\n", size);
    void *ptr = ft_malloc(size);
	//show_alloc_mem();
	//ft_printf("%p returned\n", ptr);
	return (ptr);
}

void    free(void *ptr)
{
	//ft_printf("Free %p\n", ptr);
    ft_free(ptr);
}

void *realloc(void *ptr, size_t size) {
	//ft_printf("Realloc %p %i\n", ptr, size);
	void *res = ft_realloc(ptr, size);

	//ft_printf("Realloc returned %p\n", res);
	return (res);
}

#endif
