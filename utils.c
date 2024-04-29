// utils.c

#include "shell.h"

char *get_current_directory()
{
	char *cwd = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	check(cwd);
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
	check(hostname);
	exit_if(gethostname(hostname, MAX_COMMAND_LENGTH) != 0);
	return hostname;
}

char *get_which(char *command)
{
	char *path, *path_copy, *path_token, *file_path;
	int command_length, directory_length;
	struct stat buffer;

	path = getenv("PATH");

	if (path) {
		path_copy = strdup(path);
		command_length = strlen(command);
		path_token = strtok(path_copy, ":");

		while (path_token != NULL) {
			directory_length = strlen(path_token);
			file_path =
			    malloc(command_length + directory_length + 2);
			strcpy(file_path, path_token);
			strcat(file_path, "/");
			strcat(file_path, command);
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

		if (stat(command, &buffer) == 0) {
			return command;
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

char *extract_second_word(const char *str)
{
	// 跳过字符串开头的空格
	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}

	// 跳过第一个单词
	while (*str != '\0' && !isspace((unsigned char)*str)) {
		str++;
	}

	// 跳过中间的空格
	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}

	// 寻找第二个单词的结束位置
	const char *start = str;
	while (*str != '\0' && !isspace((unsigned char)*str)) {
		str++;
	}

	// 计算第二个单词的长度
	int length = str - start;

	// 分配内存保存第二个单词，并复制内容
	char *second_word = (char *)malloc(length + 1);
	if (second_word == NULL) {
		return NULL;	// 内存分配失败
	}
	strncpy(second_word, start, length);
	second_word[length] = '\0';	// 添加字符串结束符

	return second_word;
}

char *parse_path(const char *command)
{
	// 提取第二个单词
	char *second_word = extract_second_word(command);

	// 如果第二个单词以 '~' 开头，则替换为用户主目录
	if (second_word[0] == '~') {
		// 获取用户主目录
		char *home_dir = get_home_directory();
		if (home_dir != NULL) {
			// 计算新字符串的长度
			int new_length =
			    strlen(home_dir) + strlen(second_word) - 1;

			// 分配内存保存替换后的字符串
			char *parsed_path = (char *)malloc(new_length + 1);
			if (parsed_path != NULL) {
				// 拼接用户主目录和第二个单词的剩余部分
				snprintf(parsed_path, new_length + 1, "%s%s",
					 home_dir, second_word + 1);
				free(second_word);	// 释放第二个单词的内存
				return parsed_path;
			}
		}
	}

	return second_word;
}
