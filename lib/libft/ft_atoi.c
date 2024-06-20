/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: odessein <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/13 16:12:19 by odessein          #+#    #+#             */
/*   Updated: 2023/01/06 15:31:46 by odessein         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

static size_t	ft_len_nb(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (ft_is_digit(str[i]))
		i++;
	return (i);
}

static size_t	ft_skip(const char *nptr, int *i, long long *res,
					long *neg_sign)
{
	*i = 0;
	*res = 0;
	*neg_sign = 1;
	while (nptr[*i] == '\v' || nptr[*i] == '\t' || nptr[*i] == '\r'
		|| nptr[*i] == '\n' || nptr[*i] == '\f' || nptr[*i] == ' ')
		*i = *i + 1;
	return (ft_len_nb(&nptr[*i]));
}

static bool	check_sign(int *i, size_t size, const char *nptr, long *neg_sign)
{
	if (nptr[*i] == '-' || nptr[*i] == '+')
	{
		if (size > 11)
			return (true);
		if (nptr[*i] == '-')
			*neg_sign = -1;
		(*i)++;
	}
	else if (size > 10)
		return (true);
	return (false);
}

long	ft_atoi(const char *nptr)
{
	int			i;
	long long	res;
	long		neg_sign;
	size_t		size;

	size = ft_skip(nptr, &i, &res, &neg_sign);
	if (!size || check_sign(&i, size, nptr, &neg_sign))
		return (21474836489);
	while (nptr[i] != '\0' && nptr[i] != ' ' && ft_isdigit(nptr[i]))
	{
		res *= 10;
		res += nptr[i] - 48;
		i++;
	}
	if ((res * (long long) neg_sign) < -2147483648)
		return (21474836489);
	else if ((res * (long long) neg_sign) > 2147483647)
		return (21474836489);
	res *= neg_sign;
	return (res);
}
