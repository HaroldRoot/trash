// shell.c

#include "shell.h"

char startup_directory[PATH_MAX];
char history_file_path[PATH_MAX + 50];
char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;
Alias *aliases_head;
int num_aliases = 0;

void init_globals()
{
	size_t length =
	    strlen(startup_directory) + strlen("/.trash_history") + 1;
	if (length > sizeof(history_file_path)) {
		fprintf(stderr, "Error: Path length exceeds limit.\n");
		return;
	}

	snprintf(history_file_path, sizeof(history_file_path),
		 "%s/.trash_history", startup_directory);

	username = get_username();
	hostname = get_hostname();
	current_directory = get_current_directory();

	add_alias("la", "ls -A");
	add_alias("greet", "echo Hello, world!");
	add_alias("h", "help");
	num_aliases = 3;
}

void free_globals()
{
	free(username);
	free(hostname);
	free(current_directory);
}

int main()
{
	exit_if(getcwd(startup_directory, sizeof(startup_directory)) == NULL);
	printf("startup_dir: %s\n", startup_directory);

	if (isatty(STDIN_FILENO)) {
		// shell is running in interactive mode
		// display prompt and accept input from user
		init_globals();
		print_logo();
		while (1) {
			print_prompt();
			char *input = read_input();
			execute(input);
			free(input);
		}
	} else {
		// shell is running in non-interactive mode
		// execute cmds from script or batch file
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
		if (c == EOF && isatty(STDIN_FILENO)) {
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
