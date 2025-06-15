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

static void	heredoc(t_pipex *p, char **av)
{
		p->is_heredoc = true;
		p->infile = open("heredoc_.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
		if (p->infile < 0)
			handle_open_error(av[1], p);
		while (true)
		{
			input = get_next_line(STDIN_FILENO);
			if (!input)
				break ;
			if (ft_strncmp(input, av[2], ft_strlen(av[2])) == 0 && input[ft_strlen(av[2])] == '\n')
			{
				free(input);
				break ;
			}
			if (!write(p->infile, input, ft_strlen(input)))
    			perror("write");
			free(input);
		}
		close(p->infile);
		p->infile = open("heredoc_.txt", O_RDONLY);
		if (p->infile < 0)
			handle_open_error(av[1], p);
}

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
	char	*input;

	if (ac < 5)
		exit_handler(1, "Error: Invalid amount of arguments!", NULL);
	p = ft_calloc(1, sizeof(t_pipex));
	if (!p)
		exit_handler(1, "allocation failed", NULL);
	if (ft_strncmp(av[1], "here_doc", 8) == 0)
		heredoc(p, av);
	else
	{
		p->is_heredoc = false;
		p->infile = open(av[1], O_RDONLY);
		if (p->infile < 0)
			handle_open_error(av[1], p);
	}
	if (p->is_heredoc == true)
		p->outfile = open(av[ac - 1], O_WRONLY | O_APPEND | O_CREAT, 0644);
	else
		p->outfile = open(av[ac - 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (p->outfile < 0)
		handle_open_error(av[ac - 1], p);
	execute_pipeline(p, ac, av, env);
	exit_handler(p->status, NULL, p);
}
