#include "ft_malloc.h"

void	loop_mem_zone(t_mem_zone *start, size_t *allocated_bytes) {
	t_mem_zone	*buff = start;

	while (buff) {
		loop_chunk(buff->first, allocated_bytes);
		if (buff->largest_chunk) {
			ft_printf("test %i %p\n", (int)buff->largest_chunk->size, buff->largest_chunk);
		}
		buff = buff->next;
	}
}

void loop_chunk(t_chunk *first, size_t *allocated_bytes) {
	t_chunk *buff = first;
	t_chunk *next = NULL;

	while (buff) {
		next = buff->next;
		ft_printf("%p - %p: %lu bytes State %i\n", buff, next, buff->size, buff->state);
		*allocated_bytes = *allocated_bytes + buff->size;
		buff = next;
	}
}

void	show_alloc_mem() {
	t_alloc_info	info = g_alloc_info;
	size_t			allocated_bytes = 0;

	ft_printf("%s %p %i\n", "TINY: ", info.tiny, info.nb_tiny_elems);
	loop_mem_zone(info.tiny, &allocated_bytes);
	ft_printf("%s %p\n", "SMALL: ", info.small);
	loop_mem_zone(info.small, &allocated_bytes);
	ft_printf("%s %p\n", "LARGE: ", info.large);
	loop_mem_zone(info.large, &allocated_bytes);
}
