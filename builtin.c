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
	} else {
		char *equal_sign = strchr(argv[1], '=');
		if (equal_sign != NULL && equal_sign != argv[1]
		    && *(equal_sign + 1) != '\0') {
			*equal_sign = '\0';
			char *alias = argv[1];
			char *replacement = equal_sign + 1;
			char *dynamic_replacement = strdup(replacement);
			check_null(dynamic_replacement);
			strip_quotes(&dynamic_replacement);
			add_alias(alias, dynamic_replacement);
			free(dynamic_replacement);
		} else {
			fprintf(stderr, "Usage: alias <alias>=<replacement>\n");
		}
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
		print_history(0);
	} else {
		int n = atoi(argv[1]);
		if (n > 0) {
			print_history(n);
		} else {
			fprintf(stderr,
				"Error: Invalid number of commands to display.\n");
		}
	}
}

void print_history(int n)
{
	FILE *fph = fopen(history_file_path, "r");
	if (fph == NULL) {
		perror("Unable to open history file");
		return;
	}
	// 获取文件中的总行数
	int total_lines = 0;
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fph) != -1) {
		total_lines++;
	}
	rewind(fph);		// 重置文件指针到文件开头

	int line_number = 1;
	int start_line = (n > 0) ? (total_lines - n + 1) : 1;
	start_line = (start_line < 1) ? 1 : start_line;	// 确保起始行号不小于 1

	// 跳过不需要打印的行
	while (line_number < start_line && getline(&line, &len, fph) != -1) {
		line_number++;
	}

	// 打印所需的历史记录行
	while ((getline(&line, &len, fph)) != -1
	       && (n == 0 || line_number >= start_line)) {
		printf("%4d %s", line_number, line);
		line_number++;
	}

	free(line);
	fclose(fph);
}
