/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arybarsk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/05 14:04:54 by arybarsk          #+#    #+#             */
/*   Updated: 2024/02/05 14:04:57 by arybarsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"
#include "get_next_line_bonus.h"

//This is a custom rand function using /dev/urandom as source of random value.
//The random value is limited to INT_MAX in the return statement.
int	get_rand_value(t_pipex *pipex)
{
	int		random_value;
	ssize_t	bytes;

	pipex->urandom = open("/dev/urandom", O_RDONLY);
	if (pipex->urandom < 0)
		snuff_it(pipex, "Error opening /dev/urandom\n", NULL, 255);
	bytes = read(pipex->urandom, &random_value, sizeof(random_value));
	if (bytes < 0)
		snuff_it(pipex, "Error reading from /dev/urandom\n", NULL, 255);
	close_fd_set_minus1(&pipex->urandom);
	return (random_value % INT_MAX);
}

//This function converts the random value returned from the previous function
//into a random string.
//The random string is incorporated into filenames to make them unique. 
void	init_temp_file(t_pipex *pipex)
{
	char		temp_filename[MAX_FILENAME_LEN];
	char		*random_str;
	int			random_value;

	random_value = get_rand_value(pipex);
	random_str = ft_itoa(random_value);
	if (!random_str)
		snuff_it(pipex, "Error converting value to string\n", NULL, 255);
	ft_strlcpy(temp_filename, "temp_doc_", sizeof(temp_filename));
	ft_strlcat(temp_filename, random_str, sizeof(temp_filename));
	ft_strlcat(temp_filename, ".txt", sizeof(temp_filename));
	free(random_str);
	pipex->temp_filename = ft_strdup(temp_filename);
	if (!pipex->temp_filename)
		snuff_it(pipex, "Error creating temp_filename\n", NULL, 255);
	pipex->temp_file = open(pipex->temp_filename,
			O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (pipex->temp_file < 0)
		snuff_it(pipex, "Error creating temp_file\n", NULL, 255);
}

void	write_temp_file(t_pipex *pipex, char **argv)
{
	char	*buffer;
	int		delimiter_len;

	delimiter_len = ft_strlen(argv[2]);
	buffer = get_next_line(0);
	while (buffer != NULL)
	{
		if (ft_strlen(buffer) == delimiter_len
			&& ft_strncmp(buffer, argv[2], delimiter_len) == 0)
		{
			free(buffer);
			break ;
		}
		if ((write(pipex->temp_file, buffer, ft_strlen(buffer)) < 0)
			|| (write(pipex->temp_file, "\n", 1) < 0))
		{
			close_fd_set_minus1(&pipex->temp_file);
			unlink(pipex->temp_filename);
			free(buffer);
			snuff_it(pipex, "Error writing to here_doc\n", NULL, 255);
		}
		free(buffer);
		buffer = get_next_line(0);
	}
}

void	get_heredoc(t_pipex *pipex, char **argv)
{
	init_temp_file(pipex);
	write_temp_file(pipex, argv);
	close_fd_set_minus1(&pipex->temp_file);
}
