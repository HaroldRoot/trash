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

	// 确保 rl_line_buffer 不是 NULL
	if (!rl_line_buffer) {
		return NULL;
	}
	// 确保 start 和 end 在 rl_line_buffer 的有效范围内
	size_t buffer_length = strlen(rl_line_buffer);
	if (start < 0 || (size_t)start > buffer_length) {
		return NULL;
	}

	rl_attempted_completion_over = 1;	// 不使用默认的文件名补全

	// 检查是否是第一个单词或管道符后的第一个单词
	if (start == 0 ||
	    (start >= 2 && rl_line_buffer[start - 2] == '|'
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
	static char **commands = NULL;

	if (!state) {
		// 释放上一次的命令列表
		if (commands) {
			for (int i = 0; commands[i]; ++i) {
				free(commands[i]);
			}
			free(commands);
			commands = NULL;
		}

		list_index = 0;
		len = strlen(text);
		commands = get_commands();	// 获取命令列表
		if (!commands) {
			// 如果获取命令列表失败，则返回 NULL
			return NULL;
		}
	}
	// 遍历命令列表，寻找匹配项
	char *name;
	while ((name = commands[list_index])) {
		if (strncmp(name, text, len) == 0) {
			// 找到匹配项，复制并返回
			list_index++;
			return strdup(name);
		}
		list_index++;
	}

	// 如果没有更多的匹配项，重置 list_index 并准备下一次调用
	if (!commands[list_index]) {
		list_index = 0;
	}

	return NULL;		// 没有更多的匹配项
}

int is_executable(const char *path)
{
	if (path == NULL) {
		// 如果路径是 NULL，直接返回 0
		return 0;
	}

	struct stat st;
	if (stat(path, &st) == 0) {
		// 检查文件是否存在并且用户具有执行权限
		if ((st.st_mode & S_IFMT) == S_IFREG && (st.st_mode & S_IXUSR)) {
			return 1;
		}
	}
	// 如果文件不存在，或者用户没有执行权限，或者 stat 调用失败，返回 0
	return 0;
}

char **get_commands()
{
	char **commands = NULL;
	size_t num_commands = 0;

	// 获取内置命令
	for (int i = 0; i < num_builtins(); i++) {
		char **new_commands =
		    realloc(commands, sizeof(char *) * (num_commands + 2));
		if (!new_commands) {
			// 处理 realloc 失败
			free_commands(commands);
			return NULL;
		}
		commands = new_commands;
		commands[num_commands++] = strdup(builtins[i].name);
	}

	// 获取别名
	Alias *current = aliases_head;
	while (current != NULL) {
		char **new_commands =
		    realloc(commands, sizeof(char *) * (num_commands + 2));
		if (!new_commands) {
			// 处理 realloc 失败
			free_commands(commands);
			return NULL;
		}
		commands = new_commands;
		commands[num_commands++] = strdup(current->name);
		current = current->next;
	}

	// 获取 PATH 环境变量中的命令
	char *path_env = getenv("PATH");
	if (!path_env) {
		// 如果 PATH 不存在，直接返回当前命令列表
		commands[num_commands] = NULL;
		return commands;
	}

	char *paths = strdup(path_env);
	char *saveptr;
	char *path = strtok_r(paths, ":", &saveptr);
	while (path != NULL) {
		DIR *dir = opendir(path);
		if (dir) {
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL) {
				char *full_path =
				    create_full_path(path, entry->d_name);
				if (full_path && is_executable(full_path)) {
					char **new_commands = realloc(commands,
								      sizeof
								      (char *) *
								      (num_commands
								       + 2));
					if (!new_commands) {
						// 处理 realloc 失败
						free(full_path);
						closedir(dir);
						free(paths);
						free_commands(commands);
						return NULL;
					}
					commands = new_commands;
					commands[num_commands++] =
					    strdup(entry->d_name);
				}
				free(full_path);
			}
			closedir(dir);
		}
		path = strtok_r(NULL, ":", &saveptr);
	}

	commands[num_commands] = NULL;	// 结束标记
	free(paths);
	return commands;
}

// 辅助函数：释放命令列表
void free_commands(char **commands)
{
	if (commands) {
		for (int i = 0; commands[i]; i++) {
			free(commands[i]);
		}
		free(commands);
	}
}

// 辅助函数：创建完整路径
char *create_full_path(const char *dir, const char *file)
{
	int len = snprintf(NULL, 0, "%s/%s", dir, file);
	char *full_path = malloc(len + 1);
	if (full_path) {
		snprintf(full_path, len + 1, "%s/%s", dir, file);
	}
	return full_path;
}

void initialize_readline()
{
	// 设置命令补全函数
	rl_attempted_completion_function = command_completion;
}

void sigchld_handler(int signum)
{
	(void)signum;

	// 暂存 readline 的状态
	char *saved_line = NULL;
	int saved_point = rl_point;

	if (rl_line_buffer) {
		saved_line = strdup(rl_line_buffer);
		rl_save_prompt();
		rl_replace_line("", 0);
		rl_redisplay();
	}

	int status;
	pid_t pid;

	// 使用 WNOHANG 非阻塞地检查子进程状态
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		if (WIFEXITED(status)) {
			printf("\n[%d] done       %s\n", pid,
			       get_cmd_by_pid(pid));
			remove_bg_job(pid);	// 从后台作业列表中移除
		}
	}

	// 恢复 readline 的状态
	if (saved_line) {
		rl_restore_prompt();
		rl_replace_line(saved_line, 0);
		rl_point = saved_point;
		rl_redisplay();
		free(saved_line);
	}

	fflush(stdout);
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

	signal(SIGCHLD, sigchld_handler);
}
