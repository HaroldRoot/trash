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

void save_history(char *input)
{
	// 检查输入是否为空
	if (input == NULL || strlen(input) == 0) {
		return;
	}
	// 检查历史文件路径是否存在，如果不存在则创建
	struct stat st = { 0 };
	if (stat(history_file_path, &st) == -1) {
		FILE *fp = fopen(history_file_path, "w");
		if (fp == NULL) {
			perror("Unable to create history file");
			return;
		}
		fclose(fp);
	}
	// 检查输入是否与最后一条历史记录相同
	FILE *fph = fopen(history_file_path, "r");
	if (fph == NULL) {
		perror("Unable to open history file");
		return;
	}

	char last_line[PATH_MAX] = { 0 };
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fph) != -1) {
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';	// 移除换行符
		}
		strcpy(last_line, line);
	}
	free(line);
	fclose(fph);

	// 如果输入与最后一条历史记录不同，则保存
	if (strcmp(last_line, input) != 0) {
		fph = fopen(history_file_path, "a");
		if (fph == NULL) {
			perror("Unable to open history file");
			return;
		}

		fprintf(fph, "%s\n", input);
		fclose(fph);
	}

	trim_history();
}

void trim_history()
{
	// 打开历史文件以读取命令
	FILE *fph = fopen(history_file_path, "r");
	if (fph == NULL) {
		perror("Unable to open history file for trimming");
		return;
	}
	// 计算文件中的总行数
	int total_lines = 0;
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fph) != -1) {
		total_lines++;
		free(line);	// 释放 getline 分配的内存
		line = NULL;	// 重置指针，防止 double free
	}
	fclose(fph);

	// 如果历史记录超过了 HISTSIZE，删除最早的记录
	if (total_lines > HISTSIZE) {
		// 重新打开历史文件以读取和写入
		fph = fopen(history_file_path, "r");
		if (fph == NULL) {
			perror("Unable to open history file for trimming");
			return;
		}

		FILE *fp_temp = fopen("/tmp/temp_history", "w");	// 在 /tmp 目录中创建临时文件
		if (fp_temp == NULL) {
			perror
			    ("Unable to open temporary history file for trimming");
			fclose(fph);	// 确保关闭已打开的文件
			return;
		}

		int line_to_keep = total_lines - HISTSIZE;
		int current_line = 0;

		// 跳过不需要保留的行
		while (current_line < line_to_keep
		       && getline(&line, &len, fph) != -1) {
			current_line++;
			free(line);	// 释放 getline 分配的内存
			line = NULL;	// 重置指针
		}

		// 将剩余的行写入临时文件，同时忽略空行
		while (getline(&line, &len, fph) != -1) {
			if (strlen(line) > 1) {	// 忽略空行
				fprintf(fp_temp, "%s", line);
			}
			free(line);	// 释放 getline 分配的内存
			line = NULL;	// 重置指针
		}

		fclose(fph);	// 关闭历史文件
		fclose(fp_temp);	// 关闭临时文件

		// 替换旧的历史文件
		if (remove(history_file_path) != 0
		    || rename("/tmp/temp_history", history_file_path) != 0) {
			perror("Error replacing the history file");
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
