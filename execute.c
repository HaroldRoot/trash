// execute.c

#include "shell.h"

extern char **environ;

void execute(char *cmd)
{
	char *actual = expand_alias(cmd);
	char **argv = tokenize(actual);

	int in_fd = -1, out_fd = -1, err_fd = -1;
	int stdin_copy = dup(STDIN_FILENO);
	exit_if(stdin_copy < 0);
	int stdout_copy = dup(STDOUT_FILENO);
	exit_if(stdout_copy < 0);
	int stderr_copy = dup(STDERR_FILENO);
	exit_if(stderr_copy < 0);

	handle_redirections(argv, &in_fd, &out_fd, &err_fd);

	// Check if the command should run in the background
	int run_in_background = 0;
	int argc = 0;
	while (argv[argc] != NULL) {
		if (strcmp(argv[argc], "&") == 0) {
			run_in_background = 1;
			argv[argc] = NULL;	// Remove the '&' from arguments
			break;
		}
		argc++;
	}

	if (handle_builtin(argv) != 0) {
		handle_external(argv, actual, run_in_background);
	}

	free(actual);

	restore_std(in_fd, out_fd, err_fd, stdin_copy, stdout_copy,
		    stderr_copy);
}

void handle_external(char **argv, char *cmd, int run_in_background)
{
	(void)cmd;

	ExecuteResult result = execute_external(argv, run_in_background);

	if (result == EXECUTE_FAILURE) {
		fprintf(stderr, "External command execution failed\n");
	}
}

ExecuteResult execute_external(char **argv, int run_in_background)
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
		_exit(EXIT_FAILURE);
	} else {
		if (!run_in_background) {
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
		} else {
			add_bg_job(pid, detokenize(argv));
			printf("Running in background, PID: %d\n", pid);
			return EXECUTE_SUCCESS;
		}
	}
}
