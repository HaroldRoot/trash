// utils.c

#include "shell.h"

char *get_current_directory()
{
	char *cwd = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	check_null(cwd);
	exit_if(getcwd(cwd, MAX_COMMAND_LENGTH) == NULL);
	// Replace user home directory path with ~
	const char *home_dir = get_home_directory();
	size_t home_dir_len = strlen(home_dir);
	if (strncmp(cwd, home_dir, home_dir_len) == 0) {
		cwd[0] = '~';
		memmove(cwd + 1, cwd + home_dir_len,
			strlen(cwd + home_dir_len) + 1);
	}

	return cwd;
}

char *get_home_directory()
{
	// 使用 getenv() 获取 HOME 环境变量
	char *home_dir = getenv("HOME");
	if (home_dir != NULL) {
		return home_dir;
	}
	// 如果获取失败，尝试使用 getpwuid() 获取
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
	if (pw != NULL) {
		return pw->pw_dir;
	}
	// 如果两种方式都无法获取，则返回 NULL
	return NULL;
}

char *get_username()
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	exit_if(pw == NULL);
	return pw->pw_name;
}

char *get_hostname()
{
	char *hostname = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	check_null(hostname);
	exit_if(gethostname(hostname, MAX_COMMAND_LENGTH) != 0);
	return hostname;
}

char *get_which(char *cmd)
{
	char *path, *path_copy, *path_token, *file_path;
	int cmd_length, directory_length;
	struct stat buffer;

	path = getenv("PATH");

	if (path) {
		path_copy = strdup(path);
		cmd_length = strlen(cmd);
		path_token = strtok(path_copy, ":");

		while (path_token != NULL) {
			directory_length = strlen(path_token);
			file_path = malloc(cmd_length + directory_length + 2);
			strcpy(file_path, path_token);
			strcat(file_path, "/");
			strcat(file_path, cmd);
			strcat(file_path, "\0");

			if (stat(file_path, &buffer) == 0) {
				free(path_copy);
				return file_path;
			} else {
				free(file_path);
				path_token = strtok(NULL, ":");
			}
		}

		free(path_copy);

		if (stat(cmd, &buffer) == 0) {
			return cmd;
		}

		return NULL;
	}

	return NULL;
}

char *trim_leading_space(char *str)
{
	while (isspace((unsigned char)*str))
		str++;
	return str;
}

char *parse_path(char *path)
{
	if (path[0] != '~')
		return path;

	// 获取用户主目录
	char *home_dir = get_home_directory();
	if (home_dir != NULL) {
		// 计算新字符串的长度
		int new_length = strlen(home_dir) + strlen(path) - 1;

		// 分配内存保存替换后的字符串
		char *parsed_path = (char *)malloc(new_length + 1);
		if (parsed_path != NULL) {
			// 拼接用户主目录和第二个单词的剩余部分
			snprintf(parsed_path, new_length + 1, "%s%s",
				 home_dir, path + 1);
			return parsed_path;
		}
	}

	return path;
}

void strip_quotes(char **word)
{
	size_t len = strlen(*word);
	if (len >= 2 && ((*word)[0] == '\'' || (*word)[0] == '\"')
	    && (*word)[0] == (*word)[len - 1]) {
		memmove(*word, *word + 1, len - 2);
		(*word)[len - 2] = '\0';
		*word = realloc(*word, len - 1);
	}
}
