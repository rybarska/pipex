/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_children.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 20:39:17 by arybarsk          #+#    #+#             */
/*   Updated: 2023/11/29 20:39:20 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include "get_next_line_bonus.h"

static void	protect_fork(t_pipex *pipex)
{
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index + 1]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index]);
	snuff_it(pipex, "Error forking\n", NULL, 255);
}

static void	handle_parent(t_pipex *pipex)
{
	t_node	*new_node;

	if (pipex->index < pipex->cmd_num - 1)
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index + 1]);
	if (pipex->index > 0)
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
	if (pipex->pid < 0)
		snuff_it(pipex, "Error forking processes\n", NULL, 255);
	new_node = (t_node *)malloc(sizeof(t_node));
	if (!new_node)
		snuff_it(pipex, "Error allocating memory\n", NULL, 255);
	new_node->child_pid = pipex->pid;
	new_node->status = -1;
	new_node->next = NULL;
	if (!pipex->child_list_tail)
	{
		pipex->child_list_head = new_node;
		pipex->child_list_tail = new_node;
	}
	else
	{
		pipex->child_list_tail->next = new_node;
		pipex->child_list_tail = pipex->child_list_tail->next;
	}
}

void	make_child(t_pipex *pipex, char *command)
{
	if (pipex->cmd_array)
		free_array(pipex->cmd_array);
	if (pipex->found_path)
		free(pipex->found_path);
	if (pipex->index < pipex->cmd_num - 1
		&& pipe(pipex->pipe + 2 * pipex->index) < 0)
		snuff_it(pipex, "Error creating pipe fds\n", NULL, 255);
	pipex->pid = fork();
	if (pipex->pid < 0)
		protect_fork(pipex);
	if (pipex->pid == 0)
	{
		get_single_command_path(pipex, command);
		if (pipex->index == 0)
			exec_first_command(pipex, command);
		else if (pipex->index == pipex->cmd_num - 1)
			exec_last_command(pipex, command);
		else
			exec_middle_command(pipex, command);
	}
	else if (pipex->pid != 0)
	{
		handle_parent(pipex);
	}
}
