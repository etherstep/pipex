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
//#include <asm-generic/errno-base.h>
//#include <asm-generic/errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void	handle_open_error(char *filename, t_pipex *p)
{
	if (errno == EISDIR)
		ft_printf(STDERR_FILENO, "%s: Is a directory\n", filename);
	else if (errno == ENOTDIR)
		ft_printf(STDERR_FILENO, "%s: Not a directory\n", filename);
	else if (errno == EACCES)
		ft_printf(STDERR_FILENO, "%s: Permission denied\n", filename);
	else if (errno == ENOENT)
	{
		ft_printf(STDERR_FILENO, "%s: No such file or directory\n", filename);
		exit_handler(1, NULL, p);
	}
}

int	main(int ac, char **av, char **env)
{
	t_pipex	*p;

	if (ac < 5)
		exit_handler(1, "Error: Invalid amount of arguments!", NULL);
	p = ft_calloc(1, sizeof(t_pipex));
	if (!p)
		exit_handler(1, "allocation failed", NULL);
	p->infile = open(av[1], O_RDONLY);
	p->outfile = open(av[ac - 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (p->infile < 0)
		handle_open_error(av[1], p);
	if (p->outfile < 0)
		handle_open_error(av[ac - 1], p);
	execute_pipeline(p, ac, av, env);
	exit_handler(p->status, NULL, p);
}
