/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 22:20:34 by jpelline          #+#    #+#             */
/*   Updated: 2025/05/27 22:21:24 by jpelline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
// # include <asm-generic/errno-base.h>
// # include <asm-generic/errno.h>
# include <signal.h>
# include <stdbool.h>

# ifndef PIPELINE_LIMIT
#  define PIPELINE_LIMIT 512
# endif

enum pipe
{
	READ = 0,
	WRITE = 1,
};

typedef struct s_pipex
{
	int		infile;
	int		outfile;
	int		fd_cmd;
	int		status;
	pid_t	*pid;
	int		**pipefd;
	char	*path;
	int		pipe_index;
	int		pipe_count;
	int		cmd_count;
	char	**cmd_args;
}			t_pipex;

void	execute_pipeline(t_pipex *p, int ac, char **av, char **env);
void	exit_handler(int code, char *param1, void *param2);
void	free_handler_exit(t_pipex *p, char **array1, char **array2, bool status);
void	child_process(t_pipex *p, char *av, char **env);
void	get_cmd_args(t_pipex *p, char *av_cmd);
void	get_bin_path(t_pipex *p, char *av_index, char **env);
#endif // PIPEX_H
