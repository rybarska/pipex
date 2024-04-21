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

#include "a_pipex.h"

static void	handle_bad_infile(t_pipex *pipex)
{
	if (access(pipex->argv[1], F_OK) != 0)
		snuff_it(pipex, "Error: file does not exist\n", pipex->argv[1], 1);
	else if (access(pipex->argv[1], R_OK | X_OK) != 0)
		snuff_it(pipex, "Error opening file\n", pipex->argv[1], 1);
	else
		snuff_it(pipex, "Error: bad file\n", pipex->argv[1], 1);
}

static void	exec_last_command(t_pipex *pipex, char *command)
{
	close_fd_set_minus1(&pipex->infile);
	if (pipex->outfile < 0)
	{
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
		snuff_it(pipex, "Error: permission denied\n", pipex->argv[4], 1);
	}
	if (dup2(pipex->pipe[2 * pipex->index - 2], 0) < 0
		|| dup2(pipex->outfile, 1) < 0)
	{
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
		if (dup2(pipex->pipe[2 * pipex->index - 2], 0) < 0)
			snuff_it(pipex, "Error redirecting last pipe\n", NULL, 255);
		else
			snuff_it(pipex, "Error redirecting outfile\n", NULL, 255);
	}
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
	close_fd_set_minus1(&pipex->outfile);
	if (!pipex->found_path || !pipex->cmd_array[0])
		snuff_it(pipex, "Error: command not found\n", command, 127);
	if (execve(pipex->found_path, pipex->cmd_array, environ) == -1)
		snuff_it(pipex, "Is a directory or error executing last command\n",
			command, 126);
}

void	exec_first_command(t_pipex *pipex, char *command)
{
	close_fd_set_minus1(&pipex->outfile);
	close_fd_set_minus1(&pipex->pipe[0]);
	if (dup2(pipex->pipe[1], 1) < 0)
		snuff_it(pipex, "Error redirecting pipe[1]\n", NULL, 255);
	close_fd_set_minus1(&pipex->pipe[1]);
	if (pipex->infile >= 0)
	{
		if (dup2(pipex->infile, 0) < 0)
			snuff_it(pipex, "Error redirecting infile\n", NULL, 255);
		close_fd_set_minus1(&pipex->infile);
	}
	else
		handle_bad_infile(pipex);
	if (!pipex->found_path || !pipex->cmd_array[0])
		snuff_it(pipex, "Error: command not found\n", command, 127);
	if (execve(pipex->found_path, pipex->cmd_array, environ) == -1)
		snuff_it(pipex, "Is a directory or error executing first command\n",
			command, 126);
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
	{
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index + 1]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index]);
		snuff_it(pipex, "Error forking\n", NULL, 255);
	}
	if (pipex->pid == 0)
	{
		get_single_command_path(pipex, command);
		if (pipex->index == 0)
			exec_first_command(pipex, command);
		else if (pipex->index == pipex->cmd_num - 1)
			exec_last_command(pipex, command);
	}
	else if (pipex->pid != 0)
		handle_parent(pipex);
}
