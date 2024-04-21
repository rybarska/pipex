/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   a_utils.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 15:25:02 by arybarsk          #+#    #+#             */
/*   Updated: 2024/04/08 15:25:04 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "a_pipex.h"

void	close_std_fds(t_pipex *pipex)
{
	close_fd_set_minus1(&pipex->stdin_fd);
	close_fd_set_minus1(&pipex->stdout_fd);
	close_fd_set_minus1(&pipex->stderr_fd);
}

void	pipex_lstclear(t_node **lst, void (*del)(void *))
{
	t_node	*current;
	t_node	*next;

	if (!lst || !del)
		return ;
	current = *lst;
	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
	*lst = NULL;
}
