/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 18:00:31 by arybarsk          #+#    #+#             */
/*   Updated: 2024/04/07 18:00:35 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include "get_next_line_bonus.h"

static void	handle_bad_infile(t_pipex *pipex)
{
	if (access(pipex->argv[1], F_OK) != 0)
		snuff_it(pipex, "Error: file does not exist\n", pipex->argv[1], 1);
	else if (access(pipex->argv[1], R_OK | X_OK) != 0)
		snuff_it(pipex, "Error opening file\n", pipex->argv[1], 1);
	else
		snuff_it(pipex, "Error: bad file\n", pipex->argv[1], 1);
}

static void	handle_bad_outfile(t_pipex *pipex)
{
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
	snuff_it(pipex, "Error: permission denied\n",
		pipex->argv[pipex->cmd_num + 2 + pipex->here_doc], 1);
}

void	exec_last_command(t_pipex *pipex, char *command)
{
	close_fd_set_minus1(&pipex->infile);
	if (pipex->outfile < 0)
		handle_bad_outfile(pipex);
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

void	exec_middle_command(t_pipex *pipex, char *command)
{
	close_fd_set_minus1(&pipex->infile);
	close_fd_set_minus1(&pipex->outfile);
	if (dup2(pipex->pipe[2 * pipex->index - 2], 0) < 0
		|| dup2(pipex->pipe[2 * pipex->index + 1], 1) < 0)
	{
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index + 1]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index]);
		close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
		if (dup2(pipex->pipe[2 * pipex->index - 2], 0) < 0)
			snuff_it(pipex, "Error redirecting pipe to stdout\n", NULL, 255);
		else
			snuff_it(pipex, "Error redirecting pipe to stdin\n", NULL, 255);
	}
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 2]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index + 1]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index]);
	close_fd_set_minus1(&pipex->pipe[2 * pipex->index - 1]);
	if (!pipex->found_path || !pipex->cmd_array[0])
		snuff_it(pipex, "Error: command not found\n", command, 127);
	if (execve(pipex->found_path, pipex->cmd_array, environ) == -1)
		snuff_it(pipex, "Is a directory or error executing command\n",
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
