#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define MAX_COMMAND_LENGTH 1024
#define PROMPT_FORMAT "[%s@%s %s]$ "

// Global pointers for username, hostname, and current directory
char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;

void init_globals();
void free_globals();
void print_prompt();
char *get_current_directory();
char *get_username();
char *get_hostname();
void execute_command(char *command);
int is_builtin_command(char *command);
void handle_builtin_command(char *command);

void init_globals()
{
	// Initialize global pointers for username, hostname, and current directory
	username = get_username();
	hostname = get_hostname();
	current_directory = get_current_directory();
}

void free_globals()
{
	// Free memory allocated for global variables
	free(username);
	free(hostname);
	free(current_directory);
}

void print_prompt()
{
	printf(PROMPT_FORMAT, username, hostname, current_directory);
}

char *get_current_directory()
{
	char *cwd = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	if (cwd == NULL) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	if (getcwd(cwd, MAX_COMMAND_LENGTH) == NULL) {
		perror("getcwd() error");
		exit(EXIT_FAILURE);
	}
	// Replace user home directory path with ~
	struct passwd *pw = getpwuid(getuid());
	const char *home_dir = pw->pw_dir;
	size_t home_dir_len = strlen(home_dir);

	if (strncmp(cwd, home_dir, home_dir_len) == 0) {
		cwd[0] = '~';
		memmove(cwd + 1, cwd + home_dir_len,
			strlen(cwd + home_dir_len) + 1);
	}

	return cwd;
}

char *get_username()
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw == NULL) {
		perror("getpwuid() error");
		exit(EXIT_FAILURE);
	}
	return pw->pw_name;
}

char *get_hostname()
{
	char *hostname = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	if (hostname == NULL) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	if (gethostname(hostname, MAX_COMMAND_LENGTH) != 0) {
		perror("gethostname() error");
		exit(EXIT_FAILURE);
	}
	return hostname;
}

void execute_command(char *command)
{
	if (is_builtin_command(command)) {
		handle_builtin_command(command);
	} else {
		// Implement executing non-builtin commands here
		system(command);
	}
}

int is_builtin_command(char *command)
{
	if (strcmp(command, "exit") == 0) {
		printf("Exiting trash...\n");
		return 1;
	}
	return 0;
}

void handle_builtin_command(char *command)
{
	if (strcmp(command, "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
}

void cleanup()
{
	// Free dynamically allocated memory
	char *username = get_username();
	char *hostname = get_hostname();
	char *current_directory = get_current_directory();

	free(username);
	free(hostname);
	free(current_directory);
}

int main()
{
	char command[MAX_COMMAND_LENGTH];

	// Initialize global variables
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
