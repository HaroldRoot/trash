// init.c

#include "shell.h"

char startup_directory[PATH_MAX - 20];
char history_file_path[PATH_MAX];
char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;
Alias *aliases_head;
int num_aliases = 0;

void init()
{
	getcwd(startup_directory, sizeof(startup_directory));
	snprintf(history_file_path, sizeof(history_file_path),
		 "%s/.trash_history", startup_directory);

	username = get_username();
	hostname = get_hostname();
	current_directory = get_current_directory();

	add_alias("greet", "echo Hello, world!");
	add_alias("la", "ls -A");
	add_alias("h", "help");
	num_aliases = 3;

	signal(SIGUSR1, signal_handler);

	print_logo();
	printf("TRASH startup is completed.\n");
	printf("Type 'help' for more information.\n");
}
