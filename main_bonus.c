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

#include "pipex_bonus.h"
#include "get_next_line_bonus.h"

void	set_empty(t_pipex *pipex)
{
	pipex->here_doc = -1;
	pipex->temp_file = -1;
	pipex->temp_filename = NULL;
	pipex->urandom = -1;
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

void	parse_args_with_heredoc(t_pipex *pipex, int argc, char **argv)
{
	int	i;

	i = -1;
	if (argc < 6)
	{
		write(2, "Error: not enough arguments for parsing with here_doc\n", 53);
		exit(EXIT_FAILURE);
	}
	pipex->here_doc = 1;
	pipex->cmd_num = argc - 4;
	pipex->outfile = open(argv[argc - 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
	get_heredoc(pipex, argv);
	pipex->infile = open(pipex->temp_filename, O_RDONLY);
	if (pipex->infile < 0)
		unlink(pipex->temp_filename);
}

void	parse_args_no_heredoc(t_pipex *pipex, int argc, char **argv)
{
	int	i;

	i = -1;
	if (argc < 5)
	{
		write(2, "Error: not enough arguments\n", 28);
		exit(EXIT_FAILURE);
	}
	pipex->here_doc = 0;
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

	set_empty(&pipex);
	pipex.argv = argv;
	if (ft_strncmp(argv[1], "here_doc", 9) == 0)
		parse_args_with_heredoc(&pipex, argc, argv);
	else
		parse_args_no_heredoc(&pipex, argc, argv);
	get_command_paths(&pipex);
	pipex.pipe_fd_num = 2 * (pipex.cmd_num - 1);
	pipex.pipe = (int *)malloc(sizeof(int) * pipex.pipe_fd_num);
	if (!(pipex.pipe))
		snuff_it(&pipex, "Error allocating memory for pipe", NULL, 255);
	while (++(pipex.index) < pipex.cmd_num)
		make_child(&pipex, argv[pipex.index + argc - 1 - pipex.cmd_num]);
	last_exit_code = wait_for_children(&pipex);
	close_std_fds(&pipex);
	exit_like_a_boss(&pipex, last_exit_code);
}
