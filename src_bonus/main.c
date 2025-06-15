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
		if (p->fd3 >= 0)
			close(p->fd3);
		if (p->fd4 >= 0)
			close(p->fd4);
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

// void	child_process(t_pipex *p, int *pipefd, char *av, char **env)
// {
// 	dup2(pipefd[0], STDIN_FILENO);
// 	dup2(p->fd2, STDOUT_FILENO);
// 	close(pipefd[1]);
// 	close(pipefd[0]);
// 	get_bin_path(p, av, env);
// 	if (!p->path)
// 		exit_handler(127, "Error", p);
// 	p->cmd_args = get_cmd_args(p, av);
// 	if (!p->cmd_args)
// 	{
// 		if (p->fd4 > 0)
// 			exit_handler(126, "Error", p);
// 		exit_handler(127, "Error", p);
// 	}
// 	execve(p->path, p->cmd_args, env);
// 	exit(errno);
// }

void	child_process(t_pipex *p, int *pipefd, char *av, char **env)
{
	if (p->child == 0)
	{
		dup2(p->fd1, STDIN_FILENO);
		dup2(pipefd[1], STDOUT_FILENO);
	}
	if (p->child == 1)
	{
		dup2(p->fd2, STDOUT_FILENO);
		dup2(pipefd[0], STDIN_FILENO);
	}
	close(pipefd[1]);
	close(pipefd[0]);
	get_bin_path(p, av, env);
	if (!p->path)
		exit_handler(127, "Error", p);
	p->cmd_args = get_cmd_args(p, av);
	if (!p->cmd_args)
		exit_handler(127, "Error", p);
	execve(p->path, p->cmd_args, env);
	exit(errno);
}


int	main(int ac, char **av, char **env)
{
	int		**pipefd;
	t_pipex	*p;

	// if (ac != 5)
	// {
	// 	ft_printf(STDERR_FILENO, "Error: Invalid amount of arguments!\n");
	// 	exit_handler(1, NULL, NULL);
	// }
	p = ft_calloc(1, sizeof(t_pipex));
	if (!p)
		exit_handler(1, "Error", p);

	p->fd1 = open(av[1], O_RDONLY, 0777);
	p->fd2 = open(av[4], O_WRONLY | O_TRUNC | O_CREAT, 0777);
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
				ft_printf(STDERR_FILENO, "%s: Permssion denied\n", av[4]);
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
			ft_printf(STDERR_FILENO, "%s: Is a directory\n", av[1]);
			exit_handler(1, NULL, p);
		}
		exit_handler(1, av[4], p);
	}


	p->fd3 = open(av[2], O_DIRECTORY);
	p->fd4 = open(av[3], O_DIRECTORY);
	if (p->fd3 >= 0)
	{
		ft_printf(STDERR_FILENO, "Error: command not found: %s\n", av[2]);
		if (p->fd4 >= 0)
		{
			ft_printf(STDERR_FILENO, "Error: command not found: %s\n", av[3]);
			exit_handler(126, NULL, p);
		}
		if (p->fd4 < 0)
		{
			exit_handler(0, NULL, p);	
		}
		exit_handler(127, NULL, p);
	}

	if (p->fd4 >= 0)
	{
		ft_printf(STDERR_FILENO, "Error: command not found: %s\n", av[3]);
		exit_handler(126, NULL, p);
	}
	
	// p->pid1 = fork();
	// if (p->pid1 == 0)
	// {
	// 	dup2(p->fd1, pipefd[0]);
	// 	dup2(pipefd[0], STDIN_FILENO);
	// 	dup2(pipefd[1], STDOUT_FILENO);
	// 	close(pipefd[0]);
	// 	close(pipefd[1]);
	// 	get_bin_path(p, av[2], env);
	// 	if (!p->path)
	// 		exit_handler(127, "Error", p);
	// 	p->cmd_args = get_cmd_args(p, av[2]);
	// 	if (!p->cmd_args)
	// 		exit_handler(127, "Error", p);
	// 	execve(p->path, p->cmd_args, env);
	// 	exit(errno);
	// }
	// p->pid2 = fork();
	// if (p->pid2 == 0)
	// 	child_process(p, pipefd, av[3], env);

	// close(pipefd[0]);
	// close(pipefd[1]);
	
	int i;
	pipefd = ft_calloc(ac - 4, sizeof(int *));
	i = 0;
	while (i < ac - 4)
	{
		pipefd[i] = ft_calloc(2, sizeof(int));
		i++;
	}

	i = 0;
	while (i < ac - 4)
	{
		pipe(pipefd[i]);
		if (pipefd[i] < 0)
			exit_handler(errno, "Error", p);
		i++;
	}

	pid_t pid[ac - 3];
	i = 0;
	int j = 0;
	while (i < ac - 3)
	{
		p->child = i;
		pid[i] = fork();
		if (pid[i] == 0)
		{
			if (i > 2)
				j++;
			child_process(p, pipefd[j], av[i + 2], env);
		}
		i++;
	}

	i = 0;
	while (i < ac - 4)
	{
		close(pipefd[i][0]);
		close(pipefd[i][1]);
		i++;
	}
	i = 0;
	while(i < ac - 3)
	{
		waitpid(pid[i], &p->status, 0);
		i++;
	}


	// waitpid(p->pid1, &p->status, 0);
	// waitpid(p->pid2, &p->status, 0);


	exit_handler(p->status, NULL, p);
}
