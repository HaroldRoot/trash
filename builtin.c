// builtin.c

#include "shell.h"

const char *builtins[] = { "alias", "cd", "exit", "help (h)", "unalias" };

void handle_builtin(char **argv, CmdType t)
{
	switch (t) {
	case (BUILTIN_EXIT):
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
		break;
	case (BUILTIN_HELP):
		print_help();
		break;
	case (BUILTIN_CD_PATH):
		if (chdir(parse_path(argv[1])) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_CD_DEFAULT):
		if (chdir(get_home_directory()) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_ALIAS):
		if (argv[1] == NULL) {
			list_aliases();
		} else if (argv[1] != NULL && argv[2] != NULL) {
			add_alias(argv[1], argv[2]);
		} else {
			fprintf(stderr, "Usage: alias <alias> <replacement>\n");
		}
		break;
	case (BUILTIN_UNALIAS):
		if (argv[1] != NULL) {
			remove_alias(argv[1]);
		} else {
			fprintf(stderr, "Usage: unalias <alias>\n");
		}
		break;
	default:
		break;
		// Add other builtin cmds here
	}
}

CmdType check_cd(char **argv)
{
	if (argv[1])
		return BUILTIN_CD_PATH;
	return BUILTIN_CD_DEFAULT;
}

void print_help()
{
	print_logo();

	printf("TRASH is the abbreviation of TRAsh SHell.\n");
	printf("This is an OS course assignment for juniors.\n");
	printf("Builtin Commands are as follows:\n\n");

	int i;
	for (i = 0; i < num_builtins(); i++) {
		printf("  %s\n", builtins[i]);
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

int num_builtins()
{
	return sizeof(builtins) / sizeof(char *);
}
