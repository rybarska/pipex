/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_clean.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 20:36:58 by arybarsk          #+#    #+#             */
/*   Updated: 2023/11/29 20:37:01 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "a_pipex.h"

void	close_fd_set_minus1(int *fd)
{
	if (*fd != -1)
	{
		if (close(*fd) < 0)
			perror("Error closing file descriptor\n");
		else
			*fd = -1;
	}
}

void	free_array(char **array)
{
	int	i;

	i = -1;
	if (array)
	{
		while (array[++i])
			free(array[i]);
		free(array);
	}
}

static void	clean_up(t_pipex *pipex)
{
	int	i;

	i = 0;
	close_fd_set_minus1(&pipex->infile);
	close_fd_set_minus1(&pipex->outfile);
	if (pipex->cmd_paths)
		free_array(pipex->cmd_paths);
	if (pipex->cmd_array)
		free_array(pipex->cmd_array);
	if (pipex->found_path)
		free(pipex->found_path);
	free(pipex->pipe);
	pipex_lstclear(&(pipex->child_list_head), free);
	pipex->child_list_head = NULL;
	pipex->child_list_tail = NULL;
}

void	snuff_it(t_pipex *pipex, char *error_msg, char *name, int exit_code)
{
	clean_up(pipex);
	if (name)
	{
		write(2, name, ft_strlen(name));
		write(2, ": ", 2);
	}
	write(2, error_msg, ft_strlen(error_msg));
	exit(exit_code);
}

void	exit_like_a_boss(t_pipex *pipex, int exit_code)
{
	clean_up(pipex);
	exit(exit_code);
}
