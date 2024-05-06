// shell.c

#include "shell.h"

#define MAX_CMD 10
#define MAX_ARGC 10

int main()
{
	init();
	while (1) {
		print_prompt();
		char *input = read_input();
		process(input);
	}
	return 0;
}

char *read_input()
{
	int bufsize = BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	check_null(buffer);
	int c;
	while (1) {
		c = getchar();
		if (c == EOF) {
			printf("\nExiting trash...\n");
			exit(EXIT_SUCCESS);
		} else if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
		if (position >= bufsize) {
			bufsize += BUFSIZE;
			buffer = realloc(buffer, bufsize);
			check_null(buffer);
		}
	}
}

void signal_handler(int sig)
{
	if (sig == SIGUSR1) {
		exit(EXIT_SUCCESS);
	}
}

void process(char *input)
{
	int cmdcnt = 0;
	char *cmd[MAX_CMD][MAX_ARGC] = { NULL };
	int i = 0;

	char **argv = tokenize(input);
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
}
