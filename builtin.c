// builtin.c

#include "shell.h"

BuiltinCmd builtins[] = {
	{"exit", &exit_shell},
	{"help", &print_help},
	{"alias", &handle_alias},
	{"unalias", &handle_unalias},
	{"cd", &handle_cd},
	{"history", &handle_history}
};

int num_builtins()
{
	return sizeof(builtins) / sizeof(builtins[0]);
}

int handle_builtin(char **argv)
{
	for (int i = 0; i < num_builtins(); i++) {
		if (strcmp(argv[0], builtins[i].name) == 0) {
			builtins[i].handler(argv);
			return 0;
		}
	}
	return 1;		// not builtin cmd
}

void exit_shell(char **argv)
{
	(void)argv;
	printf("Exiting trash...\n");
	exit(EXIT_SUCCESS);
}

void print_help(char **argv)
{
	(void)argv;
	print_logo();

	printf("TRASH is the abbreviation of TRAsh SHell.\n");
	printf("This is an OS course assignment for juniors.\n");
	printf("Builtin Commands are as follows:\n\n");

	int i;
	for (i = 0; i < num_builtins(); i++) {
		printf("  %s\n", builtins[i].name);
	}

	printf("\nUse 'man' command for information on external programs.\n");
}

void print_logo()
{
	srand(time(NULL));
	int file_index = rand() % MAX_FILES;
	char filename[MAX_FILENAME_LENGTH];
	sprintf(filename, "logos/%d.txt", file_index);

	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Can't open file %s\n", filename);
		return;
	}

	int c;
	while ((c = fgetc(file)) != EOF) {
		putchar(c);
	}

	fclose(file);
	return;
}

void handle_alias(char **argv)
{
	if (argv[1] == NULL) {
		list_aliases();
	} else if (argv[1] != NULL && argv[2] != NULL) {
		add_alias(argv[1], argv[2]);
	} else {
		fprintf(stderr, "Usage: alias <alias> <replacement>\n");
	}
}

void handle_unalias(char **argv)
{
	if (argv[1] != NULL) {
		remove_alias(argv[1]);
	} else {
		fprintf(stderr, "Usage: unalias <alias>\n");
	}
}

void handle_cd(char **argv)
{
	if (chdir(argv[1] ? parse_path(argv[1]) : get_home_directory()) != 0)
		perror("cd failed");
}

void handle_history(char **argv)
{
	if (argv[1] == NULL) {
		print_history();
	}
}

void save_history(char *input)
{
	FILE *fph = fopen(history_file_path, "a");
	if (fph == NULL) {
		perror("Unable to open history file");
		return;
	}

	fprintf(fph, "%s\n", input);
	fclose(fph);
}

void print_history()
{

	FILE *fph = fopen(history_file_path, "r");
	if (fph == NULL) {
		perror("Unable to open history file");
		return;
	}

	char c = fgetc(fph);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(fph);
	}

	fclose(fph);
}
