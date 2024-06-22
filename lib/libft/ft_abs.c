#include "libft.h"

size_t	ft_abs(int val) {
	if (val < 0) {
		return ((size_t)(val * -1));
	}
	return ((size_t)val);
}
