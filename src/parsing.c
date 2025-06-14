#include "pipex.h"

void	get_cmd_args(t_pipex *p, char *av_cmd)
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
	tokens = ft_calloc(i + 1, sizeof(char *));
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
	p->cmd_args = tokens;
	free(args);
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