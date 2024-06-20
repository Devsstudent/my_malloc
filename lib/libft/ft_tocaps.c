/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tocaps.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/02 16:56:08 by odessein          #+#    #+#             */
/*   Updated: 2022/12/02 17:51:45 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_tocaps(char *str)
{
	int		i;
	int		size;
	char	*dup;

	i = 0;
	size = ft_strlen(str);
	dup = malloc(sizeof(*dup) * size + 1);
	while (i < size)
	{
		if (str[i] && str[i] < 123 && str[i] > 96)
			dup[i] = str[i] - 32;
		else
			dup[i] = str[i];
		i++;
	}
	dup[i] = 0;
	return (dup);
}
