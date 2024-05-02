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

	int i = 0;
	int out_fd = -1;
	int stdout_fd = dup(STDOUT_FILENO);
	while (argv[i] != NULL) {
		if (strcmp(argv[i], ">") == 0) {
			out_fd =
			    open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
				 0644);
			if (out_fd < 0) {
				perror("open");
				exit(EXIT_FAILURE);
			}
			if (dup2(out_fd, STDOUT_FILENO) < 0) {
				perror("dup2");
				exit(EXIT_FAILURE);
			}
			close(out_fd);
			free(argv[i]);
			argv[i] = NULL;
			free(argv[i + 1]);
			argv[i + 1] = NULL;
			break;
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
	if (stdout_fd != -1) {
		dup2(stdout_fd, STDOUT_FILENO);	// 恢复标准输出
		close(stdout_fd);	// 关闭保存的标准输出文件描述符
	}
}

void handle_external(char **argv, char *cmd)
{
	ExecuteResult result = execute_external(argv);

	if (result == EXECUTE_FAILURE) {
		system(cmd);
	}
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
