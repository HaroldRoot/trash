// tokenize.h

#ifndef TOKENIZE_H
#define TOKENIZE_H

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

char **tokenize(char *cmd);
char *detokenize(char **argv);

#endif
