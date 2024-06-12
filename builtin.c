// builtin.c

#include "shell.h"

BuiltinCmd builtins[] = {
	{"cd", &handle_cd},
	{"type", &handle_type},
	{"jobs", &handle_jobs},
	{"exit", &exit_shell},
	{"help", &print_help},
	{"alias", &handle_alias},
	{"unalias", &handle_unalias},
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

	printf("\n" BOLD "TRASH" RESET " is the abbreviation of " BOLD "Tra"
	       RESET "sh " BOLD "Sh" RESET "ell.\n");
	printf("This is an OS course assignment for juniors.\n");
	printf("Builtin Commands are as follows:\n\n");

	int i;
	for (i = 0; i < num_builtins(); i++) {
		printf(BOLD YELLOW "  %s\n" RESET, builtins[i].name);
	}

	printf("\nUse " BOLD UNDERLINE CYAN "man" RESET
	       " command for information on external programs.\n\n");
}

void print_logo()
{
	srand(time(NULL));
	int file_index = 8;
	char filename[PATH_MAX];	// 使用 PATH_MAX 作为 filename 的大小

	// 检查 startup_directory 的实际长度
	size_t startup_length = strlen(startup_directory);
	if (startup_length + 15 >= PATH_MAX) {	// "/logos/%d.txt" 最长为 15 个字符
		fprintf(stderr, "Error: startup_directory is too long\n");
		return;
	}
	// 使用 snprintf 安全地构造路径
	snprintf(filename, sizeof(filename), "%s/logos/%d.txt",
		 startup_directory, file_index);

	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Can't open file %s\n", filename);
		return;
	}

	printf(MAGENTA);
	int c;
	while ((c = fgetc(file)) != EOF) {
		putchar(c);
	}
	printf(RESET);

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
		add_history(input);
		write_history(history_file_path);
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
		printf("%4d  %s", line_number, line);
		line_number++;
	}

	free(line);
	fclose(fph);
}

void handle_jobs(char **argv)
{
	(void)argv;
	print_bg_jobs();
}

void add_bg_job(pid_t pid, char *cmd)
{
	if (num_bg_jobs < MAX_JOBS) {
		bg_jobs[num_bg_jobs].pid = pid;
		bg_jobs[num_bg_jobs].cmd = cmd;
		num_bg_jobs++;
	}
}

void print_bg_jobs()
{
	printf("Background jobs:\n");
	for (int i = 0; i < num_bg_jobs; i++) {
		printf("[%d] %d %s\n", i + 1, bg_jobs[i].pid, bg_jobs[i].cmd);
	}
}

char *get_cmd_by_pid(pid_t pid)
{
	for (int i = 0; i < num_bg_jobs; i++) {
		if (bg_jobs[i].pid == pid) {
			return bg_jobs[i].cmd;
		}
	}
	return NULL;
}

void remove_bg_job(pid_t pid)
{
	for (int i = 0; i < num_bg_jobs; i++) {
		if (bg_jobs[i].pid == pid) {
			free(bg_jobs[i].cmd);	// 释放命令字符串的内存
			bg_jobs[i] = bg_jobs[num_bg_jobs - 1];	// 将最后一个作业移动到当前位置
			num_bg_jobs--;	// 减少作业数量
			return;
		}
	}
}

int is_bg_job(pid_t pid)
{
	for (int i = 0; i < num_bg_jobs; i++) {
		if (bg_jobs[i].pid == pid) {
			return 1;	// 找到了，是后台作业
		}
	}
	return 0;		// 没找到，不是后台作业
}

void handle_type(char **argv)
{
	if (argv[1] == NULL) {
		// If no argument is provided, do nothing
		return;
	}

	char *cmd = argv[1];
	Alias *alias = find_alias(cmd);
	if (alias) {
		printf("%s is an alias for %s\n", cmd, alias->replacement);
		return;
	}

	for (int i = 0; i < num_builtins(); i++) {
		if (strcmp(cmd, builtins[i].name) == 0) {
			printf("%s is a shell builtin\n", cmd);
			return;
		}
	}

	char *path = get_which(cmd);
	if (path) {
		printf("%s is %s\n", cmd, path);
		free(path);
	} else {
		printf("%s not found\n", cmd);
	}
}
