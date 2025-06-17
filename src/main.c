/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpelline <jpelline@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 22:18:10 by jpelline          #+#    #+#             */
/*   Updated: 2025/06/16 13:18:35 by jpelline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	handle_open_error(char *filename, t_pipex *p)
{
	static int	check;

	if (errno == EISDIR)
		ft_printf(STDERR_FILENO, "%s: Is a directory\n", filename);
	else if (errno == ENOTDIR)
		ft_printf(STDERR_FILENO, "%s: Not a directory\n", filename);
	else if (errno == EACCES)
		ft_printf(STDERR_FILENO, "%s: Permission denied\n", filename);
	else if (errno == ENOENT)
		ft_printf(STDERR_FILENO, "%s: No such file or directory\n", filename);
	else
		ft_printf(STDERR_FILENO, "%s: Error opening\n", filename);
	if (check == 1)
		exit_handler(0, NULL, p);
	check = 1;
	if (p->outfile < 0)
		exit_handler(1, NULL, p);
}

static void	open_infile(t_pipex *p, char **av)
{
	p->infile = open(av[1], O_RDONLY);
	if (p->infile < 0)
		handle_open_error(av[1], p);
}

static void	open_outfile(t_pipex *p, int ac, char **av)
{
	p->outfile = open(av[ac - 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (p->outfile < 0)
		handle_open_error(av[ac - 1], p);
	else if (p->infile < 0)
		exit_handler(0, NULL, p);
}

int	main(int ac, char **av, char **env)
{
	t_pipex	*p;

	if (ac != 5)
		exit_handler(1, "Error: Invalid amount of arguments!", NULL);
	p = ft_calloc(1, sizeof(t_pipex));
	if (!p)
		exit_handler(1, "allocation failed", NULL);
	open_infile(p, av);
	open_outfile(p, ac, av);
	execute_pipeline(p, ac, av, env);
	exit_handler(WEXITSTATUS(p->status), NULL, p);
}
