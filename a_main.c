/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 18:24:41 by arybarsk          #+#    #+#             */
/*   Updated: 2023/11/29 18:24:48 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "a_pipex.h"

void	set_empty(t_pipex *pipex)
{
	pipex->temp_file = -1;
	pipex->infile = -1;
	pipex->outfile = -1;
	pipex->stdin_fd = STDIN_FILENO;
	pipex->stdout_fd = STDOUT_FILENO;
	pipex->stderr_fd = STDERR_FILENO;
	pipex->argv = NULL;
	pipex->cmd_num = -1;
	pipex->cmd_paths = NULL;
	pipex->cmd_array = NULL;
	pipex->found_path = NULL;
	pipex->index = -1;
	pipex->pipe_fd_num = -1;
	pipex->pipe = NULL;
	pipex->pid = -1;
	pipex->child_list_head = NULL;
	pipex->child_list_tail = NULL;
}

void	parse_args(t_pipex *pipex, int argc, char **argv)
{
	int	i;

	i = -1;
	pipex->cmd_num = argc - 3;
	pipex->outfile = open(argv[argc - 1], O_CREAT | O_RDWR | O_TRUNC, 0644);
	pipex->infile = open(argv[1], O_RDONLY);
}

int	wait_for_children(t_pipex *pipex)
{
	t_node	*current;
	t_node	*temp;
	int		last_exit_code;

	current = pipex->child_list_head;
	last_exit_code = 0;
	while (current != NULL)
	{
		if (waitpid(current->child_pid, &current->status, WUNTRACED) < 0)
			snuff_it(pipex, "Error: waitpid failed\n", NULL, 255);
		if (WIFSIGNALED(current->status))
			last_exit_code = WTERMSIG(current->status) + 128;
		if (WIFEXITED(current->status))
			last_exit_code = WEXITSTATUS(current->status);
		temp = current;
		current = current->next;
		free(temp);
	}
	pipex->child_list_head = NULL;
	pipex->child_list_tail = NULL;
	return (last_exit_code);
}

int	main(int argc, char **argv)
{
	t_pipex	pipex;
	int		last_exit_code;

	if (argc != 5)
	{
		write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 40);
		exit(EXIT_FAILURE);
	}
	set_empty(&pipex);
	parse_args(&pipex, argc, argv);
	pipex.argv = argv;
	get_command_paths(&pipex);
	pipex.pipe_fd_num = 2 * (pipex.cmd_num - 1);
	pipex.pipe = (int *)malloc(sizeof(int) * pipex.pipe_fd_num);
	if (!(pipex.pipe))
		snuff_it(&pipex, "Error allocating memory for pipe\n", NULL, 255);
	while (++(pipex.index) < pipex.cmd_num)
		make_child(&pipex, argv[pipex.index + argc - 1 - pipex.cmd_num]);
	last_exit_code = wait_for_children(&pipex);
	close_std_fds(&pipex);
	exit_like_a_boss(&pipex, last_exit_code);
}
