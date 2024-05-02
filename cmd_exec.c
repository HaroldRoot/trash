// cmd_exec.c

#include "shell.h"

#define CLOSE_AND_FREE(fd) \
	do { \
		if (fd != -1) { \
			close(fd); \
			fd = -1; \
		} \
		if (argv[i]) { \
			free(argv[i]); \
			argv[i] = NULL; \
		} \
		if (argv[i + 1]) { \
			free(argv[i + 1]); \
			argv[i + 1] = NULL; \
		} \
	} while (0)

extern char **environ;

void execute(char *cmd)
{
	cmd = trim_leading_space(cmd);
	if (strlen(cmd) == 0)
		return;

	char *actual = expand_alias(cmd);

	char **argv = parse(actual);
	if (argv[0] != NULL) {
		save_history(cmd);
	}

	int i = 0;
	int in_fd = -1, out_fd = 1, err_fd = -1;
	int stdout_fd = dup(STDOUT_FILENO);
	exit_if(stdout_fd == -1);
	int stderr_fd = dup(STDERR_FILENO);
	exit_if(stderr_fd == -1);

	while (argv[i] != NULL) {
		if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "1>") == 0) {
			out_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			exit_if(out_fd < 0);
			exit_if(dup2(out_fd, STDOUT_FILENO) < 0);
			CLOSE_AND_FREE(out_fd);
		} else if (strcmp(argv[i], "2>") == 0) {
			err_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			exit_if(err_fd < 0);
			exit_if(dup2(err_fd, STDERR_FILENO) < 0);
			CLOSE_AND_FREE(err_fd);
		} else if (strcmp(argv[i], ">>") == 0
			   || strcmp(argv[i], "1>>") == 0) {
			out_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND,
				 0644);
			exit_if(out_fd < 0);
			exit_if(dup2(out_fd, STDOUT_FILENO) < 0);
			CLOSE_AND_FREE(out_fd);
		} else if (strcmp(argv[i], "2>>") == 0) {
			err_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND,
				 0644);
			exit_if(err_fd < 0);
			exit_if(dup2(err_fd, STDERR_FILENO) < 0);
			CLOSE_AND_FREE(err_fd);
		} else if (strcmp(argv[i], "2>&1") == 0) {
			exit_if(dup2(stdout_fd, STDERR_FILENO) < 0);
			free(argv[i]);
			argv[i] = NULL;
		} else if (strcmp(argv[i], "<") == 0) {
			in_fd = open(argv[i + 1], O_RDONLY);
			exit_if(in_fd < 0);
			exit_if(dup2(in_fd, STDIN_FILENO) < 0);
			CLOSE_AND_FREE(in_fd);
		}
		i++;
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

	dup2(stdout_fd, STDOUT_FILENO);
	close(stdout_fd);
	dup2(stderr_fd, STDERR_FILENO);
	close(stderr_fd);
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
		free(actual);
		return EXECUTE_FAILURE;
	} else if (pid == 0) {
		execve(actual, argv, environ);
		perror("execve");
		exit(EXIT_FAILURE);
	} else {
		free(actual);
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
