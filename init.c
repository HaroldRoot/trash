// init.c

#include "shell.h"

char startup_directory[PATH_MAX - 20];
char history_file_path[PATH_MAX];
char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;
Alias *aliases_head;
int num_aliases = 0;
BgJob bg_jobs[MAX_JOBS];
int num_bg_jobs = 0;

// 命令补全函数
char **command_completion(const char *text, int start, int end)
{
	(void)end;

	rl_attempted_completion_over = 1;	// 不使用默认的文件名补全

	// 检查是否是第一个单词或管道符后的第一个单词
	if (start == 0 ||
	    (start >= 1 && rl_line_buffer[start - 2] == '|'
	     && rl_line_buffer[start - 1] == ' ')) {
		return rl_completion_matches(text, command_generator);
	} else {
		// 否则，使用文件名补全
		return rl_completion_matches(text,
					     rl_filename_completion_function);
	}
}

// 命令生成器函数
char *command_generator(const char *text, int state)
{
	static int list_index, len;
	static char **commands;
	if (!state) {
		list_index = 0;
		len = strlen(text);
		commands = get_commands();	// 获取命令列表
	}

	char *name;
	while ((name = commands[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}

	return NULL;		// 没有更多的匹配项
}

int is_executable(const char *path)
{
	struct stat st;
	if (stat(path, &st) == 0 && st.st_mode & S_IXUSR) {
		return 1;
	}
	return 0;
}

char **get_commands()
{
	// 先获取内置命令和假名
	char **commands = NULL;
	size_t num_commands = 0;

	for (int i = 0; i < num_builtins(); i++) {
		commands =
		    realloc(commands, sizeof(char *) * (num_commands + 2));
		commands[num_commands++] = strdup(builtins[i].name);
	}

	Alias *current = aliases_head;
	while (current != NULL) {
		commands =
		    realloc(commands, sizeof(char *) * (num_commands + 2));
		commands[num_commands++] = strdup(current->name);
		current = current->next;
	}

	char *path_env = getenv("PATH");
	if (path_env == NULL) {
		return NULL;
	}
	// 复制 PATH 环境变量，因为 strtok 会修改原字符串
	char *paths = strdup(path_env);
	if (paths == NULL) {
		return NULL;
	}
	// 分割 PATH 字符串为单独的路径
	char *path = strtok(paths, ":");
	while (path != NULL) {
		DIR *dir = opendir(path);
		if (dir != NULL) {
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL) {
				// 构建完整的文件路径
				char *full_path;
				int len = snprintf(NULL, 0, "%s/%s", path,
						   entry->d_name);
				full_path = malloc(len + 1);
				if (full_path == NULL) {
					// 处理内存分配失败
					closedir(dir);
					free(paths);
					return NULL;
				}
				snprintf(full_path, len + 1, "%s/%s", path,
					 entry->d_name);

				// 检查文件是否可执行
				if (is_executable(full_path)) {
					// 添加到命令列表
					commands =
					    realloc(commands,
						    sizeof(char *) *
						    (num_commands + 2));
					if (commands == NULL) {
						// 如果内存分配失败，应该处理错误
						closedir(dir);
						free(paths);
						return NULL;
					}
					commands[num_commands] =
					    strdup(entry->d_name);
					num_commands++;
				}
				free(full_path);
			}
			closedir(dir);
		}
		path = strtok(NULL, ":");
	}

	// 添加 NULL 作为数组的结束标记
	commands[num_commands] = NULL;

	free(paths);
	return commands;
}

void initialize_readline()
{
	// 设置命令补全函数
	rl_attempted_completion_function = command_completion;
}

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

	print_logo();
	printf(BOLD "TRASH" RESET " startup is completed.\n");
	printf("Type " BOLD UNDERLINE YELLOW "help" RESET
	       " for more information.\n\n");

	initialize_readline();
	using_history();
	read_history(history_file_path);
}
