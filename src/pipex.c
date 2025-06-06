/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 22:18:10 by jpelline          #+#    #+#             */
/*   Updated: 2025/05/27 22:18:52 by jpelline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <fcntl.h>
#include <unistd.h>

void    exit_handler(void)
{
    perror("Error");
    exit(EXIT_FAILURE);
}

char	**get_cmd_args(char **av, char *path)
{
    char **cmd_args = NULL;
    char **cmd1;

    printf("test");
    cmd1 = ft_split(av[2], ' ');
    int i = 1;
    cmd_args[0] = ft_strdup(path);
    while (cmd1[i] != NULL)
    {
	cmd_args[i] = ft_strdup(cmd1[i]);
	i++;
    }
    printf("%s\n", cmd_args[0]);
    return (cmd_args);
}

char	*get_bin_path(char **av, char **env)
{
    char **env_paths;
    char *current_path;
    char **cmd1;
    int i;
    i = 0;
    while (env[i])
    {
	if (ft_strnstr(env[i], "PATH=", 5))
	    break ;
	i++;
    }
    env_paths = ft_split(env[i] + 5, ':');
    cmd1 = ft_split(av[2], ' ');
    cmd1[0] = ft_strjoin("/", cmd1[0]);
    i = 0;
    while (env_paths[i] != NULL)
    {
	current_path = ft_strjoin(env_paths[i], cmd1[0]);
	if (access(current_path, X_OK) == -1)
	    i++;
	else
	{
	    free(env_paths);
	    env_paths = NULL;
	    free(cmd1);
	    cmd1 = NULL;
	    return (current_path);
	    i++;
	}
    }
    return (NULL);
}

void	free_handler(char **array)
{
    int i = 0;
    while (array[i])
    {
	free(array[i]);
	i++;
    }
    free(array);
    array = NULL;
}

int main(int ac, char **av, char **env)
{
    //int	pipefd[2];
    int status;
    pid_t cmd1;
    //pid_t cmd2;
    (void)env;

    if (ac != 5)
    	exit_handler();
    open(av[1], O_WRONLY | O_APPEND | O_CREAT, 0644);
    open(av[4], O_WRONLY | O_APPEND | O_CREAT, 0644);
    //pipe(pipefd);
    cmd1 = fork();
    //cmd2 = fork();

    if (cmd1 == 0)
    {
	printf("child\n");
	//dup2(pipefd[0], 0);
	//close(pipefd[1]);
	char *path = get_bin_path(av, env);
	printf("%s\n", path);
	printf("%s\n", path);
	printf("%s\n", path);
	printf("%s\n", path);
	printf("%s\n", path);
	printf("%s\n", path);
	printf("%s\n", path);
	char **cmd_args = get_cmd_args(av, path);
	printf("%s\n", cmd_args[0]);
	execve(path, cmd_args, env);
	free(path);
	path = NULL;
	free_handler(cmd_args);
    }
    else
    {
	waitpid(cmd1, &status, 0);
	printf("parent\n");
	//dup2(pipefd[0], 0);
	//close(pipefd[1]);
	//execve("/bin/wc", , env);
    }
    //close(pipefd[0]);
    //close(pipefd[1]);
    return (0);
}
