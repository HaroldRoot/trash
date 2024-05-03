// cmd_exec.c

#include "shell.h"

#define remove_argv(); \
	free(argv[i]); \
	argv[i] = NULL; \
	free(argv[i + 1]); \
	argv[i + 1] = NULL; \
	j = i + 2; \
	while (argv[j] != NULL) { \
		argv[j - 2] = argv[j]; \
		argv[j] = NULL; \
		j++; \
	}

extern char **environ;

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

	int i = 1;
	int j = i + 2;

	while (argv[i] != NULL) {
		if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "1>") == 0) {
			out_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			exit_if(out_fd < 0);
			exit_if(dup2(out_fd, STDOUT_FILENO) < 0);
			remove_argv();
		} else if (strcmp(argv[i], "2>") == 0) {
			err_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			exit_if(err_fd < 0);
			exit_if(dup2(err_fd, STDERR_FILENO) < 0);
			remove_argv();
		} else if (strcmp(argv[i], ">>") == 0
			   || strcmp(argv[i], "1>>") == 0) {
			out_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND,
				 0644);
			exit_if(out_fd < 0);
			exit_if(dup2(out_fd, STDOUT_FILENO) < 0);
			remove_argv();
		} else if (strcmp(argv[i], "2>>") == 0) {
			err_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND,
				 0644);
			exit_if(err_fd < 0);
			exit_if(dup2(err_fd, STDERR_FILENO) < 0);
			remove_argv();
		} else if (strcmp(argv[i], "2>&1") == 0) {
			exit_if(dup2(STDOUT_FILENO, STDERR_FILENO) < 0);
			free(argv[i]);
			argv[i] = NULL;
			j = i + 1;
			while (argv[j] != NULL) {
				argv[j - 1] = argv[j];
				argv[j] = NULL;
				j++;
			}
		} else if (strcmp(argv[i], "<") == 0) {
			in_fd = open(argv[i + 1], O_RDONLY);
			exit_if(in_fd < 0);
			exit_if(dup2(in_fd, STDIN_FILENO) < 0);
			remove_argv();
		} else {
			i++;
		}
	}

	if (handle_builtin(argv) != 0) {
		handle_external(argv, actual);
	}

	free(actual);
	i = 0;
	while (argv[i] != NULL) {
		free(argv[i]);
		i++;
	}
	free(argv);

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
