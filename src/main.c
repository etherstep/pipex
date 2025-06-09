/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 22:18:10 by jpelline          #+#    #+#             */
/*   Updated: 2025/06/09 21:46:06 by jpelline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void	exit_handler(void)
{
	perror("Error");
	exit(EXIT_FAILURE);
}

char	**get_cmd_args(char *av_cmd, char *av_file, char *path)
{
	char	**cmd_args;
	char	**cmd1;
	int		i;

	cmd_args = NULL;
	cmd1 = NULL;
	cmd1 = ft_split(av_cmd, ' ');
	i = 1;
	cmd_args = malloc(5 * sizeof(char *));
	cmd_args[0] = ft_strdup(path);
	while (cmd1[i] != NULL)
	{
		cmd_args[i] = ft_strdup(cmd1[i]);
		i++;
	}
	cmd_args[i++] = ft_strdup(av_file);
	cmd_args[i] = NULL;
	return (cmd_args);
}

char	*get_bin_path(char *av_index, char **env)
{
	char	**env_paths;
	char	*current_path;
	char	**cmd1;
	int		i;

	i = 0;
	while (env[i])
	{
		if (ft_strnstr(env[i], "PATH=", 5))
			break ;
		i++;
	}
	env_paths = ft_split(env[i] + 5, ':');
	if (!env_paths)
		return (NULL);
	cmd1 = ft_split(av_index, ' ');
	if (!cmd1)
		return (NULL);
	cmd1[0] = ft_strjoin("/", cmd1[0]);
	i = 0;
	while (env_paths[i] != NULL)
	{
		current_path = ft_strjoin(env_paths[i], cmd1[0]);
		if (access(current_path, X_OK) == -1)
			i++;
		else
		{
			// free(env_paths);
			// env_paths = NULL;
			// free(cmd1);
			// cmd1 = NULL;
			return (current_path);
			i++;
		}
		free(current_path);
		current_path = NULL;
	}
	return (NULL);
}

void	free_handler(char **array)
{
	int	i;

	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
	array = NULL;
}

int	main(int ac, char **av, char **env)
{
	int		status;
	pid_t	cmd1;
	pid_t	cmd2;
	char	*path;
	char	**cmd_args;
	int		pipefd[2];
	int		fd1;
	int		fd2;

	if (ac != 5)
	{
		if (ac > 5)
			errno = E2BIG;
		else
			errno = EINVAL;
		exit_handler();
	}

	fd1 = open(av[1], O_WRONLY | O_APPEND | O_CREAT, 0644);
	fd2 = open(av[4], O_WRONLY | O_APPEND | O_CREAT, 0644);

	pipe(pipefd);
	cmd1 = fork();
	if (cmd1 == 0)
	{
		printf("child1 - start\n");
		fflush(stdout);
		dup2(pipefd[0], 0);
		close(pipefd[1]);
		path = get_bin_path(av[2], env);
		if (!path)
			exit_handler();
		cmd_args = get_cmd_args(av[2], av[1], path);
		execve(path, cmd_args, env);
		free(path);
		path = NULL;
		free_handler(cmd_args);
	}
	else
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(cmd1, &status, 0);
		printf("parent1 - start\n");
	}



	cmd2 = fork();
	if (cmd2 == 0)
	{
		printf("child2 - start\n");
		fflush(stdout);
		dup2(pipefd[1], 1);
		close(pipefd[0]);
		path = get_bin_path(av[3], env);
		if (!path)
			exit_handler();
		cmd_args = get_cmd_args(av[3], av[4], path);
		execve(path, cmd_args, env);
		free(path);
		path = NULL;
		free_handler(cmd_args);
	}
	else
	{	
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(cmd2, &status, 0); 
		printf("parent2 - start\n");
	}

	return (0);
}
