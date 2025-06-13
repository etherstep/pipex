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

enum pipe
{
	READ = 0,
	WRITE = 1,
};

typedef struct s_pipex
{
	int		fd1;
	int		fd2;
	int		fd3;
	int		fd4;
	int		status;
	pid_t	*pid;
	int		**pipefd;
	char	*path;
	int		pipefd_index;
	int		pipe_count;
	int		cmd_count;
	char	**cmd_args;
}			t_pipex;

#endif // PIPEX_H
