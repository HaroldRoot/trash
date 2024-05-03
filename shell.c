// shell.c

#include "shell.h"

int main()
{
	init();
	while (1) {
		print_prompt();
		char *input = read_input();
		execute(input);
		free(input);
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
