#include "pipex.h"

void	exit_handler(int code, char *param1, void *param2)
{
	t_pipex	*p;
	int		i;

	p = param2;
	if (p)
	{
		if (p->infile >= 0)
			close(p->infile);
		if (p->outfile >= 0)
			close(p->outfile);
		if (p->path)
			free(p->path);
		if (p->pid)
			free(p->pid);
		i = 0;
		if (p->cmd_args)
		{
			while (p->cmd_args[i])
				free(p->cmd_args[i++]);
			free(p->cmd_args);
		}
		i = 0;
        if (p->pipefd)
        {
            while (i < p->pipe_count && p->pipefd[i])
            {
                free(p->pipefd[i]);
                i++;
            }
            free(p->pipefd);
        }
		if (p->is_heredoc == true)
			if (unlink("./heredoc_.txt") == -1)
    			ft_printf(STDERR_FILENO, "unlink failed\n");
	}
	if (param1)
	{
		if (code == 21 && ft_strchr(param1, '/'))
			ft_printf(STDERR_FILENO, "%s: Is a directory\n", param1);
		else if (code == 21)
			ft_printf(STDERR_FILENO, "%s: command not found\n", param1);
		else
			ft_printf(STDERR_FILENO, "%s\n", param1);
	}
	code = (((code) & 0xff00) >> 8);
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
		exit_handler(errno, NULL, p);
	return ;
}
