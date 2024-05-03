// tokenize.c

#include "shell.h"

#define APPEND_CHAR() \
	do { \
		size_t len = strlen(word); \
		word = realloc(word, len + 2); \
		word[len] = *p; \
		word[len + 1] = '\0'; \
	} while(0)

typedef enum {
	IN_WORD,
	NOT_IN_WORD,
	IN_SINGLE_QUOTING,
	IN_DOUBLE_QUOTING
} State;

char **tokenize(char *cmd)
{
	char **argv = malloc(MAX_WORDS * sizeof(char *));
	int argc = 0;
	char *word = NULL;
	State state = NOT_IN_WORD;

	for (char *p = cmd; *p != '\0'; p++) {
		switch (state) {
		case NOT_IN_WORD:
			if (!isspace((unsigned char)*p)) {
				word = malloc(2);
				word[0] = *p;
				word[1] = '\0';
				if (*p == '\'') {
					state = IN_SINGLE_QUOTING;
				} else if (*p == '\"') {
					state = IN_DOUBLE_QUOTING;
				} else {
					state = IN_WORD;
				}
			}
			break;

		case IN_WORD:
			if (isspace((unsigned char)*p)) {
				if (word) {
					strip_quotes(&word);
					argv[argc++] = word;
					word = NULL;
				}
				state = NOT_IN_WORD;
			} else {
				APPEND_CHAR();
				if (*p == '\'') {
					state = IN_SINGLE_QUOTING;
				} else if (*p == '\"') {
					state = IN_DOUBLE_QUOTING;
				}
			}
			break;

		case IN_SINGLE_QUOTING:
			APPEND_CHAR();
			if (*p == '\'') {
				state = IN_WORD;
			}
			break;

		case IN_DOUBLE_QUOTING:
			APPEND_CHAR();
			if (*p == '\"') {
				state = IN_WORD;
			}
			break;
		}
	}

	if (word) {
		strip_quotes(&word);
		argv[argc++] = word;
	}
	argv[argc] = NULL;	// Null-terminate the array

	return argv;
}

char *detokenize(char **argv)
{
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	// 首先，计算所需的总长度
	size_t total_length = 0;
	for (int i = 0; i < argc && argv[i] != NULL; ++i) {
		total_length += strlen(argv[i]) + 1;	// 加1为了空格
	}

	// 分配足够的内存来存储合并后的字符串，包括终止符 '\0'
	char *cmd = malloc(total_length);
	if (cmd == NULL) {
		perror("malloc failed");
		return NULL;
	}
	// 初始化第一个字符为终止符，以便开始使用 strcat
	cmd[0] = '\0';

	// 遍历字符串数组，将每个字符串拼接到结果中
	for (int i = 0; i < argc && argv[i] != NULL; ++i) {
		strcat(cmd, argv[i]);
		// 如果不是最后一个字符串，添加一个空格
		if (argv[i + 1] != NULL) {
			strcat(cmd, " ");
		}
	}

	return cmd;
}
