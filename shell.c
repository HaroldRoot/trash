// shell.c

#include "shell.h"

int main()
{
	init();
	char *input = NULL, *prompt_str;
	while (1) {
		prompt_str = prompt();
		input = readline(prompt_str);
		free(prompt_str);
		if (input == NULL) {
			printf("Exiting trash...\n");
			break;
		}
		if (*input) {
			process(input);
		}
		input = NULL;
	}
	return 0;
}

void process(char *raw_input)
{
	char *input = trim_leading_spaces(raw_input);
	if (input == NULL || *input == '\0') {
		free(input);	// 如果输入为空或全是空格，释放内存并返回
		return;
	}

	char *processed_input = trim_spaces(input);
	free(input);		// 释放由 trim_leading_spaces 分配的内存
	if (processed_input == NULL || *processed_input == '\0') {
		free(processed_input);	// 如果处理后的输入为空，释放内存并返回
		return;
	}

	if (strlen(processed_input) == 0)
		return;

	save_history(processed_input);

	int cmdcnt = 0;
	char *cmd[MAX_CMD][MAX_ARGC] = { NULL };
	int i = 0;

	char *actual = expand_alias(processed_input);
	char **argv = tokenize(actual);
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	for (int arg = 0; arg < argc; arg++) {
		if (strcmp(argv[arg], "|") == 0) {
			cmd[cmdcnt][i] = NULL;
			cmdcnt++;
			i = 0;
		} else {
			cmd[cmdcnt][i] = replace_env_vars(argv[arg]);
			i++;
		}
	}
	cmd[cmdcnt][i] = NULL;
	cmdcnt++;

	if (cmdcnt == 1) {
		char *expanded_command = detokenize(cmd[0]);
		execute(expanded_command);
		free(expanded_command);
		return;
	}

	int pipefds[2 * (cmdcnt - 1)];
	for (int i = 0; i < 2 * (cmdcnt - 1); i++) {
		pipefds[i] = -1;
	}

	for (i = 0; i < (cmdcnt - 1); i++) {
		if (pipe(pipefds + i * 2) < 0) {
			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}

	int stdin_copy = dup(STDIN_FILENO);
	int stdout_copy = dup(STDOUT_FILENO);

	for (i = 0; i < cmdcnt; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			if (i != 0) {
				if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) <
				    0) {
					perror("dup2");
					exit(EXIT_FAILURE);
				}
			}
			if (i != cmdcnt - 1) {
				if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
					perror("dup2");
					exit(EXIT_FAILURE);
				}
			}
			for (int j = 0; j < 2 * (cmdcnt - 1); j++) {
				close(pipefds[j]);
			}
			char *expanded_command = detokenize(cmd[i]);
			execute(expanded_command);
			free(expanded_command);
			dup2(stdin_copy, STDIN_FILENO);
			dup2(stdout_copy, STDOUT_FILENO);
			exit(EXIT_SUCCESS);
		} else if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < 2 * (cmdcnt - 1); i++) {
		close(pipefds[i]);
	}

	for (i = 0; i < cmdcnt; i++) {
		wait(NULL);
	}

	free(processed_input);
	free(actual);
}

char *replace_env_vars(char *arg)
{
	if (arg[0] != '$') {
		return arg;	// Not an environment variable
	}

	if (strcmp(arg, "$SHELL") == 0) {
		char *shell_path =
		    malloc(strlen(startup_directory) + strlen("/shell") + 1);
		strcpy(shell_path, startup_directory);
		strcat(shell_path, "/shell");
		return shell_path;
	}

	char *env_value = getenv(arg + 1);	// Skip the '$' character
	if (env_value) {
		return strdup(env_value);
	}

	return arg;		// Return the original argument if not an environment variable
}
