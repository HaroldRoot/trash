// shell.c

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
	if (isatty(STDIN_FILENO)) {
		// shell is running in interactive mode
		// display prompt and accept input from user
		char command[MAX_COMMAND_LENGTH];

		init_globals();

		while (1) {
			print_prompt();

			if (fgets(command, sizeof(command), stdin) == NULL) {
				// Handle Ctrl+D or EOF
				printf("\nExiting trash...\n");
				free_globals();	// Free memory before exiting
				exit(EXIT_SUCCESS);
			}

			execute_command(command);
		}
	} else {
		// shell is running in non-interactive mode
		// execute commands from script or batch file
	}

	return 0;
}
