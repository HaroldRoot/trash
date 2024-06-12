// prompt.c

#include "shell.h"

char *prompt()
{
	char prompt_symbol = '$';
	char *PROMPT_FORMAT =
	    "[" BOLD BLUE "%s@%s " RESET BOLD GREEN "%s" RESET "]%c ";
	if (geteuid() == 0) {
		prompt_symbol = '#';
		PROMPT_FORMAT =
		    "[" BOLD RED "%s@%s " RESET BOLD GREEN "%s" RESET "]%c ";
	}
	// 计算提示符字符串的长度
	const char *username = get_username();
	const char *hostname = get_hostname();
	const char *current_directory = get_current_directory();
	int prompt_length = snprintf(NULL, 0, PROMPT_FORMAT, username, hostname,
				     current_directory, prompt_symbol);

	// 分配空间用于提示符字符串
	char *prompt_str = malloc(prompt_length + 1);	// 加1为了'\0'
	if (prompt_str == NULL) {
		fprintf(stderr,
			"Unable to allocate memory for prompt string.\n");
		exit(EXIT_FAILURE);
	}
	// 创建提示符字符串
	sprintf(prompt_str, PROMPT_FORMAT, username, hostname,
		current_directory, prompt_symbol);
	return prompt_str;
}
