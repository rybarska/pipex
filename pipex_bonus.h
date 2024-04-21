/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 13:52:42 by arybarsk          #+#    #+#             */
/*   Updated: 2023/11/15 13:54:08 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/wait.h>
# include <limits.h>
# include "get_next_line_bonus.h"

extern char	**environ;

# define MAX_FILENAME_LEN 42

typedef struct s_node
{
	pid_t			child_pid;
	int				status;
	struct s_node	*next;
}					t_node;

typedef struct s_pipex
{
	int		here_doc;
	int		temp_file;
	char	*temp_filename;
	int		urandom;
	int		infile;
	int		outfile;
	int		stdin_fd;
	int		stdout_fd;
	int		stderr_fd;
	char	**argv;
	int		cmd_num;
	char	**cmd_paths;
	char	**cmd_array;
	char	*found_path;
	int		index;
	int		pipe_fd_num;
	int		*pipe;
	pid_t	pid;
	t_node	*child_list_head;
	t_node	*child_list_tail;
}			t_pipex;

/* libft */
int				ft_strlen(const char *str);
int				ft_strncmp(char	*s1, char *s2, unsigned int n);
char			*ft_strdup(const char *src);
char			*ft_substr(char const *s, unsigned int start, size_t len);
char			*ft_strnstr(const char *big, const char *little, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
char			**ft_split(const char *s, const char c);
char			*ft_itoa(int n);
unsigned int	ft_strlcpy(char *dest, char *src, unsigned int size);
unsigned int	ft_strlcat(char *dest, char *src, unsigned int size);
/* utils */
void			pipex_lstclear(t_node **lst, void (*del)(void *));
void			close_std_fds(t_pipex *pipex);
/* clean */
void			close_fd_set_minus1(int *fd);
void			free_array(char **array);
void			snuff_it(t_pipex *pipex, char *error_msg, char *name,
					int exit_code);
void			exit_like_a_boss(t_pipex *pipex, int exit_code);
/* path */
void			get_command_paths(t_pipex *pipex);
void			get_single_command_path(t_pipex *pipex, char *command);
/* children */
void			make_child(t_pipex *pipex, char *command);
/* exec */
void			exec_first_command(t_pipex *pipex, char *command);
void			exec_middle_command(t_pipex *pipex, char *command);
void			exec_last_command(t_pipex *pipex, char *command);
/* here_doc */
void			get_heredoc(t_pipex *pipex, char **argv);

#endif
