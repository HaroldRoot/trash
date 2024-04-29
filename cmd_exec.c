// cmd_exec.c

#include "shell.h"

void execute_command(InputBuffer *input_buffer)
{
	char *command = input_buffer->buffer;
	command = trim_leading_space(command);
	if (strlen(command) == 0)
		return;
	CommandType t = type_of(command);
	if (t == NOT_BUILTIN) {
		handle_external_command(input_buffer);
	} else if (t == PIPELINE) {
		system(input_buffer->buffer);
	} else {
		handle_builtin_command(command, t);
	}
}

CommandType type_of(const char *command)
{
	if (strncmp(command, "exit", 4) == 0) {
		return check_exit(command);
	} else if (strncmp(command, "cd", 2) == 0) {
		return check_cd(command);
	} else if (strchr(command, '|') != NULL) {
		return PIPELINE;
	} else {
		return NOT_BUILTIN;
	}
}

void handle_external_command(InputBuffer *input_buffer)
{
	char **argv = parse_external_command(input_buffer);

	ExecuteResult result = execute_external_command(argv);

	if (result == EXECUTE_FAILURE) {
		system(input_buffer->buffer);
	}

	int i = 0;
	while (argv[i] != NULL) {
		free(argv[i]);
		i++;
	}
	free(argv);
}

char **parse_external_command(InputBuffer *input_buffer)
{
	char *full_command = strdup(input_buffer->buffer);
	check(full_command);

	char *copy_command = malloc(input_buffer->input_length);
	check(copy_command);
	strcpy(copy_command, full_command);

	const char *delim = " \t\r\n";

	int num_tokens = 0;
	char *token = strtok(full_command, delim);
	while (token != NULL) {
		num_tokens++;
		token = strtok(NULL, delim);
	}
	num_tokens++;

	char **argv = malloc(sizeof(char *) * num_tokens);
	check(argv);
	token = strtok(copy_command, delim);
	int i;
	for (i = 0; token != NULL; i++) {
		argv[i] = malloc(strlen(token) + 1);
		check(argv[i]);
		strcpy(argv[i], token);
		token = strtok(NULL, delim);
	}
	argv[i] = NULL;

	free(full_command);
	free(copy_command);

	return argv;
}

ExecuteResult execute_external_command(char **argv)
{
	if (!argv) {
		return EXECUTE_FAILURE;
	}

	char *command = argv[0];
	char *actual_command = get_which(command);

	pid_t pid = fork();

	if (pid == -1) {
		// Error occurred during fork
		return EXECUTE_FAILURE;
	} else if (pid == 0) {
		// Child process
		execve(actual_command, argv, NULL);
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
		} else {
			// Child process exited abnormally
			return EXECUTE_FAILURE;
		}
	}
}
