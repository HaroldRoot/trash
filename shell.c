// shell.c

#include "shell.h"

#define MAX_CMD 10
#define MAX_ARGC 10

int main()
{
	init();
	char *input, *prompt_str;
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
		free(input);
	}
	return 0;
}

void process(char *raw_input)
{
	char *input = trim_spaces(raw_input);
	free(raw_input);
	if (strlen(input) == 0)
		return;

	save_history(input);

	int cmdcnt = 0;
	char *cmd[MAX_CMD][MAX_ARGC] = { NULL };
	int i = 0;

	char *actual = expand_alias(input);
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
			cmd[cmdcnt][i] = argv[arg];
			i++;
		}
	}
	cmd[cmdcnt][i] = NULL;
	cmdcnt++;

	if (cmdcnt == 1) {
		execute(actual);
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
			execute(detokenize(cmd[i]));
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

	free(input);
	free(actual);
}
