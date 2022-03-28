#include "minishell.h"

int	check_redirection(t_job *job, int quit)
{
	int	fd[2];
	int	i;

	i = 0;
	fd[0] = 0;
	fd[1] = 0;
	while (job->file && job->file[i])
	{
		if (ft_strcmp(job->file[i], "<") == 0)
			fd[0] = open_file(job->file[++i], 2, quit);
		else if (ft_strcmp(job->file[i], ">") == 0)
			fd[1] = open_file(job->file[++i], 1, quit);
		else if (ft_strcmp(job->file[i], ">>") == 0)
			fd[1] = open_file(job->file[++i], 0, quit);
		else if (ft_strcmp(job->file[i], "<<") == 0)
			dup2(job->fd[0], STDIN_FILENO);
		i++;
	}
	if (fd[0] == -1 || fd[1] == -1)
		return (1);
	if (fd[0])
		dup2(fd[0], STDIN_FILENO);
	if (fd[1])
		dup2(fd[1], STDOUT_FILENO);
	return (0);
}


int	check_builtins(char **arg)
{
	if (ft_strcmp(arg[0], "echo") == 0
		||ft_strcmp(arg[0], "cd") == 0
		||ft_strcmp(arg[0], "pwd") == 0
		||ft_strcmp(arg[0], "export") == 0
		||ft_strcmp(arg[0], "unset") == 0
		||ft_strcmp(arg[0], "env") == 0
		||ft_strcmp(arg[0], "exit") == 0
		return (0);
	else
		return (1);
}

void	restore_fd(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, 0);
	close(saved_stdin);
	dup2(saved_stdout, 1);
	close(saved_stdout);
}

int	ms_builtins(char **arg, int i, t_job *job, c_data *c_data)
{
	if (arg)
	{
		if (ft_strcmp(arg[0], "echo") == 0)
			c_data->exit_status = built_echo(arg, c_data);
		else if (ft_strcmp(arg[0], "cd") == 0)
			c_data->exit_status = built_cd(arg[1], c_data);
		else if (ft_strcmp(arg[0], "pwd") == 0)
			c_data->exit_status = built_pwd();
		else if (ft_strcmp(arg[0], "export") == 0)
			c_data->exit_status = built_export(arg + 1, c_data);
		else if (ft_strcmp(arg[0], "unset") == 0)
			c_data->exit_status = built_unset(arg + 1, c_data);
		else if (ft_strcmp(arg[0], "env") == 0)
			c_data->exit_status = built_env(c_data);
		else if (ft_strcmp(arg[0], "exit") == 0)
			ms_exit(arg + 1, job);
		else
			return (1);
	}
	if (i == 0)
		return (0);
	free_exit(job);
	exit(0);
}
int	ms_exec_builtins(t_job *job, c_data *c_data)
{
	int	saved_stdin;
	int	saved_stdout;

	saved_stdin = dup(0);
	saved_stdout = dup(1);
	if (job && job->next == NULL)
	{
		if (job->cmd && check_builtins(job->cmd) == 1)
			return (0);
		if (check_redirection(job, 1) == 1)
			return (1);
		if (ms_builtins(job->cmd, 0, job) == 0)
		{
			restore_fd(saved_stdin, saved_stdout);
			return (1);
		}
	}
	return (0);
}


void	init_pipe(t_job *job)
{
	while (job)
	{
		pipe(job->fd);
		job = job->next;
	}
}

void	child_process(t_job *job, t_job *first)
{

	job->pid = fork();
	if (job->pid == -1)
		printf("Dang! This fork didn't work!");
	if (job->pid == 0)
	{
	
		if (job->previous != NULL)
			dup2(job->previous->fd[0], STDIN_FILENO);
		if (job->next != NULL)
			dup2(job->fd[1], STDOUT_FILENO);
		check_redirection(job, 0);
		close(job->fd[0]);
		close(job->fd[1]);
		free_fd(first);
		if (job->cmd && ms_builtins(job->cmd, 1, first) == 1)
			execute(job->cmd, first);
	}
	if (job->previous != NULL)
		close(job->previous->fd[0]);
	close(job->fd[1]);
}
void	ms_exec(t_job *job, c_data *c_data)
{
	t_job	*first;
	int		status;

	first = job;
	init_pipe(first);
	if (ms_exec_builtins(job, c_data) == 1)
		return ;
	if (job && job->cmd)
	{
		while (job)
		{
			child_process(job, first);
			job = job->next;
			first = ms_head_list_job(first);
		}
		while (first)
		{
			waitpid(first->pid, &status, 0);
			if (WIFEXITED(status))
				c_data->exit_status = WEXITSTATUS(status);
			first = first->next;
		}
	}
	
}