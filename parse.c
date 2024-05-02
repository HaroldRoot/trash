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

char **parse(char *cmd)
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
