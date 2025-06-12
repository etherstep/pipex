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
#include <asm-generic/errno.h>
#include <fcntl.h>
#include <stdio.h>

void	exit_handler(int code, char *param)
{
	errno = code;
	perror(param);
	exit(code);
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

char	**get_cmd_args(char *av_cmd, char *av_file, char *path)
{
	char	**cmd_args;
	char	**cmd1;
	int		size;
	int		i;

	size = 0;
	cmd1 = ft_split(av_cmd, ' ');
	if (!cmd1)
		free_handler(cmd1);
	if (av_file != NULL)
		size++;
	i = 0;
	while (av_cmd[i++])
		size++;
	cmd_args = malloc(size * sizeof(char *));
	if (!cmd_args)
	{
		free_handler(cmd1);
		return (NULL);
	}
	cmd_args[0] = ft_strdup(path);
	if (!cmd_args[0])
	{
		free_handler(cmd1);
		free_handler(cmd_args);
		return (NULL);
	}
	i = 1;
	while (cmd1[i] != NULL)
	{
		cmd_args[i] = ft_strdup(cmd1[i]);
		if (!cmd_args[i])
		{
			free_handler(cmd1);
			free_handler(cmd_args);
			return (NULL);
		}
		i++;
	}
	if (av_file != NULL)
	{
		cmd_args[i] = ft_strdup(av_file);
		if (!cmd_args[i])
		{
			free_handler(cmd1);
			free_handler(cmd_args);
			return (NULL);
		}
		i++;
	}
	free_handler(cmd1);
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
	if (ft_strnstr(env[i], "PATH=", 5) == NULL)
		return (NULL);
	env_paths = ft_split(env[i] + 5, ':');
	if (!env_paths)
		return (NULL);
	cmd1 = ft_split(av_index, ' ');
	if (!cmd1)
	{
		free_handler(env_paths);
		return (NULL);
	}
	char *temp = ft_strjoin("/", cmd1[0]);
	if (!temp)
	{
		free_handler(env_paths);
		free_handler(cmd1);
		return (NULL);
	}
	free(cmd1[0]);
	cmd1[0] = temp;
	if (!cmd1[0])
	{
		free_handler(env_paths);
		free_handler(cmd1);
		return (NULL);
	}
	i = 0;
	while (env_paths[i] != NULL)
	{
		current_path = ft_strjoin(env_paths[i], cmd1[0]);
		if (!current_path)
		{
			free_handler(env_paths);
			free_handler(cmd1);
			return (NULL);
		}
		if (access(current_path, X_OK) == -1)
			i++;
		else
		{
			free_handler(env_paths);
			free_handler(cmd1);
			return (current_path);
		}
		free(current_path);
		current_path = NULL;
	}
	free_handler(cmd1);
	free_handler(env_paths);
	return (NULL);
}

int	main(int ac, char **av, char **env)
{
	int		status;
	pid_t	cmd1;
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
		exit_handler(errno, "Error");
	}
	fd1 = open(av[1], O_RDWR, 0644);
	if (fd1 < 0)
		exit_handler(2, av[1]);
	fd2 = open(av[4], O_RDWR | O_TRUNC | O_CREAT, 0644);
	if (fd1 < 0 || fd2 < 0)
	{
		close(fd1);
		close(fd2);
		errno = EISDIR;
		exit_handler(1, "Error");
	}
	pipe(pipefd);
	if (pipefd < 0)
		exit_handler(errno, "Error");
	if ((cmd1 = fork()))
	{
		if ((cmd1 = fork()))
		{
			//printf("child2 - start\n");
			fflush(stdout);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[1]);
			path = get_bin_path(av[3], env);
			if (!path)
			{
				exit_handler(1, "Error");
			}
			cmd_args = get_cmd_args(av[3], NULL, path);
			if (!cmd_args)
			{
				free(path);
				exit_handler(1, "Error");
			}
			dup2(fd2, STDOUT_FILENO);
			execve(path, cmd_args, env);
			free(path);
			path = NULL;
			free_handler(cmd_args);
			close(pipefd[0]);
			exit(errno);
		}
		//printf("child1 - start\n");
		fflush(stdout);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		path = get_bin_path(av[2], env);
		if (!path)
		{
			exit_handler(127, "Error");
		}
		cmd_args = get_cmd_args(av[2], NULL, path);
		if (!cmd_args)
		{
			free(path);
			exit_handler(1, "Error");
		}
		dup2(fd1, STDIN_FILENO);
		execve(path, cmd_args, env);
		free(path);
		path = NULL;
		free_handler(cmd_args);
		close(pipefd[1]);
		exit(errno);
	}
	else
	{
		waitpid(cmd1, &status, 0);
		//printf("parent1 - start\n");
		close(pipefd[0]);
		close(pipefd[1]);
	}
	close(fd1);
	close(fd2);
	return (0);
}
