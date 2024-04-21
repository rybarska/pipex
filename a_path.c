/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 18:29:35 by arybarsk          #+#    #+#             */
/*   Updated: 2024/02/10 18:29:36 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "a_pipex.h"

// Save all info from global environ to local_env.
// Find "PATH=" in local_env to isolate env_path.
// Split env_path to get path_array.
// Save array of paths in pipex->cmd_paths.
// Call activate_paths() to get executable paths.
// Iterate through paths, adding "/", to get executable paths.
void	get_command_paths(t_pipex *pipex)
{
	int		i;
	char	*temp;

	i = 0;
	while (environ && environ[i] != NULL)
	{
		if (ft_strncmp(environ[i], "PATH=", 5) == 0)
			break ;
		i++;
	}
	if (!environ[i])
		return ;
	pipex->cmd_paths = ft_split(environ[i] + 5, ':');
	if (!pipex->cmd_paths)
		snuff_it(pipex, "Error creating split paths\n", NULL, 255);
	i = -1;
	while (pipex->cmd_paths[++i])
	{
		temp = pipex->cmd_paths[i];
		pipex->cmd_paths[i] = ft_strjoin(pipex->cmd_paths[i], "/");
		free(temp);
		if (pipex->cmd_paths[i] == NULL)
			snuff_it (pipex, "Error activating paths\n", NULL, 255);
	}
}

void	is_full_path(t_pipex *pipex, char *command)
{
	char	*first_space;
	char	*path_to_test;
	size_t	length;

	first_space = ft_strchr(command, ' ');
	if (first_space)
	{
		length = first_space - command;
		path_to_test = ft_substr(command, 0, length);
		if (!path_to_test)
			snuff_it(pipex, "Error creating path_to_test\n", NULL, 255);
		if (access(path_to_test, R_OK | X_OK) == 0)
		{
			pipex->found_path = ft_strdup(path_to_test);
			if (!pipex->found_path)
				snuff_it(pipex, "Error duplicating string\n", NULL, 255);
		}
	}
	else if (access(command, R_OK | X_OK) == 0)
	{
		pipex->found_path = ft_strdup(command);
		if (!pipex->found_path)
			snuff_it(pipex, "Error duplicating string\n", NULL, 255);
	}
}

void	get_path_from_env(t_pipex *pipex, char *command)
{
	int		i;

	i = -1;
	pipex->cmd_array = ft_split(command, ' ');
	if (!pipex->cmd_array)
		snuff_it(pipex, "Error creating split command strings\n", command, 255);
	while (pipex->cmd_paths[++i])
	{
		pipex->found_path
			= ft_strjoin(pipex->cmd_paths[i], pipex->cmd_array[0]);
		if (access(pipex->found_path, R_OK | X_OK) == 0)
			break ;
		else
		{
			free(pipex->found_path);
			pipex->found_path = NULL;
		}
	}
}

void	get_single_command_path(t_pipex *pipex, char *command)
{
	char	*last_slash;

	last_slash = ft_strrchr(command, '/');
	if (last_slash)
	{
		pipex->cmd_array = ft_split(last_slash + 1, ' ');
		if (!pipex->cmd_array)
			snuff_it(pipex, "Error creating cmd_array\n", NULL, 255);
		is_full_path(pipex, command);
	}
	if (pipex->found_path)
		return ;
	if (!pipex->cmd_paths)
	{
		snuff_it (pipex, "Error: no such file or directory\n", command, 2);
		return ;
	}
	get_path_from_env(pipex, command);
}
