#include "malloc.h"

void	loop_mem_zone(t_mem_zone *start, t_mem_zone *end, size_t *allocated_bytes) {
	t_mem_zone	*buff = start;

	while (buff) {
		loop_chunk(buff->first, allocated_bytes);
		buff = buff->next;
	}
}

void loop_chunk(t_chunk *first, size_t *allocated_bytes) {
	t_chunk *buff = first;
	t_chunk *next = NULL;

	while (buff) {
		next = buff->next;
		ft_printf("%p - %p: %lu bytes\n", buff, next, buff->size);
		*allocated_bytes = *allocated_bytes + buff->size;
		buff = next;
	}
}

void	show_alloc_mem() {
	t_alloc_info	info = g_alloc_info;
	size_t			allocated_bytes = 0;
	t_mem_zone		*buff = NULL;


	ft_printf("%s %p %i\n", "TINY: ", info.tiny, info.nb_tiny_elems);

	loop_mem_zone(info.tiny, info.last_tiny, &allocated_bytes);
	ft_printf("%s %p\n", "SMALL: ", info.small);
	loop_mem_zone(info.small, info.last_small, &allocated_bytes);
	ft_printf("%s %p\n", "LARGE: ", info.large);
	loop_mem_zone(info.large, info.last_large, &allocated_bytes);
}
