// cmd_exec.c

#include "shell.h"

extern char **environ;

void remove_argv(char **argv, int i);
void redirect_stdout(char **argv, int i, int *out_fd);
void redirect_stderr(char **argv, int i, int *err_fd);
void redirect_stdout_append(char **argv, int i, int *out_fd);
void redirect_stderr_append(char **argv, int i, int *err_fd);
void redirect_stderr_to_stdout(char **argv, int i);
void redirect_input(char **argv, int i, int *in_fd);
void restore_std(int in_fd, int out_fd, int err_fd, int stdin_copy,
		 int stdout_copy, int stderr_copy);
void handle_redirections(char **argv, int *in_fd, int *out_fd, int *err_fd);
void free_argv(char **argv);

void execute(char *cmd)
{
	cmd = trim_leading_space(cmd);
	if (strlen(cmd) == 0)
		return;

	save_history(cmd);

	char *actual = expand_alias(cmd);
	char **argv = parse(actual);

	int in_fd = -1, out_fd = -1, err_fd = -1;
	int stdin_copy = dup(STDIN_FILENO);
	exit_if(stdin_copy < 0);
	int stdout_copy = dup(STDOUT_FILENO);
	exit_if(stdout_copy < 0);
	int stderr_copy = dup(STDERR_FILENO);
	exit_if(stderr_copy < 0);

	handle_redirections(argv, &in_fd, &out_fd, &err_fd);

	if (handle_builtin(argv) != 0) {
		handle_external(argv, actual);
	}

	free(actual);
	free_argv(argv);

	restore_std(in_fd, out_fd, err_fd, stdin_copy, stdout_copy,
		    stderr_copy);
}

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

void free_argv(char **argv)
{
	int i = 0;
	while (argv[i] != NULL) {
		free(argv[i]);
		i++;
	}
	free(argv);
}

void handle_external(char **argv, char *cmd)
{
	(void)cmd;

	ExecuteResult result = execute_external(argv);

	if (result == EXECUTE_FAILURE) {
		fprintf(stderr, "External command execution failed\n");
	}
}

ExecuteResult execute_external(char **argv)
{
	if (!argv || !argv[0]) {
		return EXECUTE_FAILURE;
	}

	char *cmd = argv[0];
	char *actual = get_which(cmd);

	if (!actual) {
		fprintf(stderr, "Command not found: %s\n", cmd);
		return EXECUTE_FAILURE;
	}

	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
		return EXECUTE_FAILURE;
	} else if (pid == 0) {
		execve(actual, argv, environ);
		perror("execve");
		exit(EXIT_FAILURE);
	} else {
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			int exit_status = WEXITSTATUS(status);
			return exit_status ==
			    0 ? EXECUTE_SUCCESS : EXECUTE_FAILURE;
		} else {
			if (WIFSIGNALED(status)) {
				int signal_number = WTERMSIG(status);
				fprintf(stderr,
					"Process was terminated by signal %d\n",
					signal_number);
			}
			return EXECUTE_FAILURE;
		}
	}
}
