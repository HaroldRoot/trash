// redirect.c

#include "shell.h"

void handle_redirections(char **argv, int *in_fd, int *out_fd, int *err_fd)
{
	int i = 1;
	while (argv[i] != NULL) {
		if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "1>") == 0) {
			redirect_stdout(argv, i, out_fd);
		} else if (strcmp(argv[i], "2>") == 0) {
			redirect_stderr(argv, i, err_fd);
		} else if (strcmp(argv[i], ">>") == 0
			   || strcmp(argv[i], "1>>") == 0) {
			redirect_stdout_append(argv, i, out_fd);
		} else if (strcmp(argv[i], "2>>") == 0) {
			redirect_stderr_append(argv, i, err_fd);
		} else if (strcmp(argv[i], "2>&1") == 0) {
			redirect_stderr_to_stdout(argv, i);
		} else if (strcmp(argv[i], "<") == 0) {
			redirect_input(argv, i, in_fd);
		} else {
			i++;
		}
	}
}

void redirect_stdout(char **argv, int i, int *out_fd)
{
	*out_fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	exit_if(*out_fd < 0);
	exit_if(dup2(*out_fd, STDOUT_FILENO) < 0);
	remove_argv(argv, i);
}

void redirect_stderr(char **argv, int i, int *err_fd)
{
	*err_fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	exit_if(*err_fd < 0);
	exit_if(dup2(*err_fd, STDERR_FILENO) < 0);
	remove_argv(argv, i);
}

void redirect_stdout_append(char **argv, int i, int *out_fd)
{
	*out_fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
	exit_if(*out_fd < 0);
	exit_if(dup2(*out_fd, STDOUT_FILENO) < 0);
	remove_argv(argv, i);
}

void redirect_stderr_append(char **argv, int i, int *err_fd)
{
	*err_fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
	exit_if(*err_fd < 0);
	exit_if(dup2(*err_fd, STDERR_FILENO) < 0);
	remove_argv(argv, i);
}

void redirect_stderr_to_stdout(char **argv, int i)
{
	exit_if(dup2(STDOUT_FILENO, STDERR_FILENO) < 0);
	free(argv[i]);
	argv[i] = NULL;
	while (argv[i + 1] != NULL) {
		argv[i] = argv[i + 1];
		argv[i + 1] = NULL;
		i++;
	}
}

void redirect_input(char **argv, int i, int *in_fd)
{
	*in_fd = open(argv[i + 1], O_RDONLY);
	exit_if(*in_fd < 0);
	exit_if(dup2(*in_fd, STDIN_FILENO) < 0);
	remove_argv(argv, i);
}

void restore_std(int in_fd, int out_fd, int err_fd, int stdin_copy,
		 int stdout_copy, int stderr_copy)
{
	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
	clearerr(stdin);
	dup2(stdout_copy, STDOUT_FILENO);
	close(stdout_copy);
	dup2(stderr_copy, STDERR_FILENO);
	close(stderr_copy);
	close(in_fd);
	close(out_fd);
	close(err_fd);
}

void remove_argv(char **argv, int i)
{
	free(argv[i]);
	argv[i] = NULL;
	free(argv[i + 1]);
	argv[i + 1] = NULL;
	while (argv[i + 2] != NULL) {
		argv[i] = argv[i + 2];
		argv[i + 2] = NULL;
		i++;
	}
}

void free_argv(char **argv)
{
	int i = 0;
	while (argv[i] != NULL) {
		free(argv[i]);
		i++;
	}
	free(argv);
}
