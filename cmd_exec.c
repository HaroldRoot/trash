// cmd_exec.c

#include "shell.h"

extern char **environ;

void execute(char *cmd)
{
	cmd = trim_leading_space(cmd);
	if (strlen(cmd) == 0)
		return;

	char *actual = expand_alias(cmd);
	if (strchr(actual, '|') != NULL) {
		system(actual);
		return;
	}

	char **argv = parse(actual);
	if (argv[0] != NULL) {
		save_history(cmd);
	}
	if (handle_builtin(argv) != 0) {
		handle_external(argv, actual);
	}

	free(actual);
	int i = 0;
	while (argv[i] != NULL) {
		free(argv[i]);
		i++;
	}
	free(argv);
}

void handle_external(char **argv, char *cmd)
{
	ExecuteResult result = execute_external(argv);

	if (result == EXECUTE_FAILURE) {
		system(cmd);
	}
}

char **parse(char *cmd)
{
	char *full = strdup(cmd);
	check_null(full);

	char *copy = strdup(full);
	check_null(copy);

	const char *delim = " \t\r\n";

	int num_tokens = 0;
	char *token = strtok(full, delim);
	while (token != NULL) {
		num_tokens++;
		token = strtok(NULL, delim);
	}
	num_tokens++;

	char **argv = malloc(sizeof(char *) * num_tokens);
	check_null(argv);
	token = strtok(copy, delim);
	int i;
	for (i = 0; token != NULL; i++) {
		argv[i] = malloc(strlen(token) + 1);
		check_null(argv[i]);
		strcpy(argv[i], token);
		token = strtok(NULL, delim);
	}
	argv[i] = NULL;

	free(full);
	free(copy);

	return argv;
}

ExecuteResult execute_external(char **argv)
{
	if (!argv) {
		return EXECUTE_FAILURE;
	}

	char *cmd = argv[0];
	char *actual = get_which(cmd);

	pid_t pid = fork();

	if (pid == -1) {
		// Error occurred during fork
		return EXECUTE_FAILURE;
	} else if (pid == 0) {
		// Child process
		execve(actual, argv, environ);
		perror("execve");
		exit(EXIT_FAILURE);
	} else {
		// Parent process
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			// Child process exited normally
			int exit_status = WEXITSTATUS(status);
			if (exit_status == 0) {
				return EXECUTE_SUCCESS;
			} else {
				return EXECUTE_FAILURE;
			}
		} else if (WIFSIGNALED(status)) {
			// Child process was terminated by a signal
			int signal_number = WTERMSIG(status);
			fprintf(stderr, "Process was terminated by signal %d\n",
				signal_number);
			return EXECUTE_FAILURE;
		} else {
			// Child process exited abnormally
			return EXECUTE_FAILURE;
		}
	}
}
