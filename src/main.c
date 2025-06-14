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
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	exit_handler(int code, char *param1, void *param2)
{
	t_pipex	*p;
	int		i;

	p = (t_pipex *)param2;
	if (p)
	{
		if (p->fd1 >= 0)
			close(p->fd1);
		if (p->fd2 >= 0)
			close(p->fd2);
		if (p->path)
			free(p->path);
		i = 0;
		if (p->cmd_args)
		{
			while (p->cmd_args[i])
				free(p->cmd_args[i++]);
			free(p->cmd_args);
		}
		free(p);
	}
	code = (((code) & 0xff00) >> 8);
	if (param1)
		perror(param1);
	exit(code);
}

void	free_handler_exit(t_pipex *p, char **array1, char **array2, bool status)
{
	int	i;

	if (array1)
	{
		i = 0;
		while (array1[i])
			free(array1[i++]);
		free(array1);
	}
	if (array2)
	{
		i = 0;
		while (array2[i])
			free(array2[i++]);
		free(array2);
	}
	if (status == true)
		exit_handler(127, NULL, p);
	return ;
}

char	**get_cmd_args(t_pipex *p, char *av_cmd)
{
	char	**tokens;
	char	**args;
	int		i;

	args = ft_split(av_cmd, ' ');
	if (!args)
		free_handler_exit(p, NULL, NULL, true);
	i = 0;
	while (args[i])
		i++;
	tokens = malloc((i + 1) * sizeof(char *));
	if (!tokens)
		free_handler_exit(p, args, NULL, true);
	tokens[0] = ft_strdup(p->path);
	if (!tokens[0])
		free_handler_exit(p, args, tokens, true);
	i = 0;
	while (args[i])
	{
		tokens[i] = ft_strdup(args[i]);
		if (!tokens[i++])
			free_handler_exit(p, args, tokens, true);
	}
	tokens[i] = NULL;
	free(args);
	return (tokens);
}

char	**parse_paths(char **env)
{
	char	**env_paths;
	int		i;

	i = 0;
	while (env[i++])
		if (ft_strnstr(env[i], "PATH=", 5))
			break ;
	if (ft_strnstr(env[i], "PATH=", 5) == NULL)
		return (NULL);
	env_paths = ft_split(env[i] + 5, ':');
	if (!env_paths)
		return (NULL);
	return (env_paths);
}

char	*find_bin_in_path(char **env_paths, char *cmd)
{
	char	*current_path;
	int		i;
	bool	check;

	i = 0;
	check = false;
	while (env_paths[i] != NULL)
	{
		current_path = ft_strjoin(env_paths[i], cmd);
		if (!current_path)
			return (NULL);
		if (access(current_path, X_OK) == -1)
			i++;
		else
		{
			check = true;
			break ;
		}
		free(current_path);
	}
	if (check == false)
		return (NULL);
	return (current_path);
}

void	get_bin_path(t_pipex *p, char *av_index, char **env)
{
	char	**env_paths;
	char	**args;
	char	*cmd;

	if (access(av_index, X_OK) != -1)
	{
		p->path = ft_strdup(av_index);
		if (!p->path)
			free_handler_exit(p, NULL, NULL, true);
		return ;
	}
	if (ft_strchr(av_index, '/'))
	{
		free_handler_exit(p, NULL, NULL, false);
		return ;
	}
	env_paths = parse_paths(env);
	if (!env_paths)
		exit_handler(127, "Error", p);
	args = ft_split(av_index, ' ');
	if (!args)
		free_handler_exit(p, env_paths, NULL, true);
	cmd = ft_strjoin("/", args[0]);
	if (!cmd)
		free_handler_exit(p, env_paths, args, true);
	p->path = find_bin_in_path(env_paths, cmd);
	if (!p->path)
	{
		free(cmd);
		free_handler_exit(p, env_paths, args, true);
	}
	free(cmd);
	free_handler_exit(p, env_paths, args, false);
}

void	child_process(t_pipex *p, char *av, char **env)
{
	if (p->pipefd_index == 0)
	{
		if (dup2(p->fd1, STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
		if (dup2(p->pipefd[0][WRITE], STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
	}
	else if (p->pipefd_index == p->pipe_count)
	{
		if (dup2(p->pipefd[p->pipefd_index - 1][READ], STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
		if (dup2(p->fd2, STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
	}
	else
	{
		if (dup2(p->pipefd[p->pipefd_index - 1][READ], STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
		if (dup2(p->pipefd[p->pipefd_index][WRITE], STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
	}
	int i = 0;
	while (i < p->cmd_count - 1)
	{
		if (close(p->pipefd[i][READ]) < 0)
			exit_handler(errno, "close failed", p);
		if (close(p->pipefd[i][WRITE]) < 0)
			exit_handler(errno, "close failed", p);
		i++;
	}
	get_bin_path(p, av, env);
	if (!p->path)
		exit_handler(127, "Error", p);
	p->cmd_args = get_cmd_args(p, av);
	if (!p->cmd_args)
		exit_handler(127, "Error", p);
	if (execve(p->path, p->cmd_args, env) < 0)
		exit_handler(errno, "execve failed", p);
	exit(errno);
}


int	main(int ac, char **av, char **env)
{
	t_pipex	*p;

	if (ac < 5)
	{
		ft_printf(STDERR_FILENO, "Error: Invalid amount of arguments!\n");
		exit_handler(1, NULL, NULL);
	}
	p = ft_calloc(1, sizeof(t_pipex));
	if (!p)
		exit_handler(1, "Error", p);

	p->fd1 = open(av[1], O_RDONLY, 0777);
	p->fd2 = open(av[ac - 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (p->fd1 < 0)
	{
		if (p->fd2 >= 0)
		{
			ft_printf(STDERR_FILENO, "%s: No such file or directory\n", av[1]);
			exit_handler(0, NULL, p);
		}
		if (errno == EISDIR)
		{
			ft_printf(STDERR_FILENO, "%s: Is a directory\n", av[1]);
			exit_handler(1, NULL, p);
		}
		if (errno == EACCES)
		{
			if (p->fd2 < 0)
				ft_printf(STDERR_FILENO, "%s: Permssion denied\n", av[ac - 1]);
			ft_printf(STDERR_FILENO, "%s: Permssion denied\n", av[1]);
			exit_handler(1, NULL, p);
		}
		ft_printf(STDERR_FILENO, "%s: No such file or directory\n", av[1]);
		exit_handler(1, NULL, p);
	}
	if (p->fd2 < 0)
	{
		if (errno == EISDIR)
		{
			ft_printf(STDERR_FILENO, "%s: Is a directory\n", av[ac - 1]);
			exit_handler(1, NULL, p);
		}
		exit_handler(1, av[ac - 1], p);
	}


	int i;
	p->cmd_count = ac - 3;
	p->pipe_count = p->cmd_count - 1;
	p->pipefd = ft_calloc(p->pipe_count, sizeof(int *));
	if (!p->pipefd)
		free_handler_exit(p, NULL, NULL, true);

	i = 0;
	while (i < p->pipe_count)
		p->pipefd[i++] = ft_calloc(2, sizeof(int));

	i = 0;
	while (i < p->pipe_count)
	{
		pipe(p->pipefd[i]);
		if (p->pipefd[i] < 0)
			exit_handler(errno, "Error", p);
		i++;
	}
	p->pid = ft_calloc(p->cmd_count, sizeof(int));

	i = 0;
	while (i < p->cmd_count)
	{
		p->pipe_index = i;
		p->pid[i] = fork();
		if (p->pid[i] == 0)
			child_process(p, av[i + 2], env);
		else
		{
			if (i > 0)
			{
				close(p->pipefd[i - 1][READ]);
				close(p->pipefd[i - 1][WRITE]);
			}
		}
		i++;
	}

	i = 0;
	while(i < p->cmd_count)
		waitpid(p->pid[i++], &p->status, 0);

	i = 0;
	while (i < p->pipe_count)
	{
		close(p->pipefd[i][READ]);
		close(p->pipefd[i][WRITE]);
		i++;
	}
	exit_handler(p->status, NULL, p);
}
