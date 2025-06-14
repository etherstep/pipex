#include "pipex.h"

static void open_handler(t_pipex *p, char *av)
{
    p->fd_cmd = open(av, O_DIRECTORY);
    if (p->fd_cmd  >= 0)
    {
        if (close(p->fd_cmd) < 0)
            exit_handler(errno, "close failed", p);
        exit_handler(21, av, p);
    }
}

void	child_process(t_pipex *p, char *av, char **env)
{
	if (p->pipe_index == 0)
	{
		if (p->infile != -1 && dup2(p->infile, STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 (stdin) failed", p);
		if (dup2(p->pipefd[0][WRITE], STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 (stdout) failed", p);
	}
	else if (p->pipe_index == p->pipe_count)
	{
		if (dup2(p->pipefd[p->pipe_index - 1][READ], STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 (stdin) failed", p);
		if (p->outfile != -1 && dup2(p->outfile, STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 (stdout) failed", p);
	}
	else
	{
		if (dup2(p->pipefd[p->pipe_index - 1][READ], STDIN_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
		if (dup2(p->pipefd[p->pipe_index][WRITE], STDOUT_FILENO) < 0)
		    exit_handler(errno, "dup2 failed", p);
	}
	int i = 0;
	while (i < p->pipe_count)
	{
		if (close(p->pipefd[i][READ]) < 0)
			exit_handler(errno, "close failed", p);
		if (close(p->pipefd[i][WRITE]) < 0)
			exit_handler(errno, "close failed", p);
		i++;
	}
    open_handler(p, av);
	get_bin_path(p, av, env);
	get_cmd_args(p, av);
	if (execve(p->path, p->cmd_args, env) < 0)
		exit_handler(errno, "execve failed", p);
	exit(errno);
}
