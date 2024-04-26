// shell.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include "shell.h"

char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;

void init_globals()
{
	username = get_username();
	hostname = get_hostname();
	current_directory = get_current_directory();
}

void free_globals()
{
	free(username);
	free(hostname);
	free(current_directory);
}

int main()
{
	char command[MAX_COMMAND_LENGTH];

	init_globals();

	while (1) {
		print_prompt();

		if (fgets(command, sizeof(command), stdin) == NULL) {
			// Handle Ctrl+D or EOF
			printf("\n");
			free_globals();	// Free memory before exiting
			exit(EXIT_SUCCESS);
		}
		// Remove trailing newline character
		command[strcspn(command, "\n")] = '\0';

		execute_command(command);
	}

	return 0;
}
