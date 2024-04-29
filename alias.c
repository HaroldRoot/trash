// alias.c

#include "shell.h"

void add_alias(const char *name, const char *replacement)
{
	if (num_aliases < MAX_ALIASES) {
		aliases[num_aliases].name = strdup(name);
		aliases[num_aliases].replacement = strdup(replacement);
		num_aliases++;
	} else {
		printf("Error: Maximum number of aliases reached.\n");
	}
}

const char *find_alias(const char *name)
{
	for (int i = 0; i < num_aliases; i++) {
		if (strcmp(aliases[i].name, name) == 0) {
			return aliases[i].replacement;
		}
	}
	return NULL;
}

char *expand_alias(const char *cmd)
{
	char *tmp = strdup(cmd);

	char *first_word = strtok(tmp, " \t\r\n");

	char *copy = strdup(cmd);

	const char *replacement = find_alias(first_word);

	if (replacement) {
		size_t replacement_len = strlen(replacement);

		char *actual = strdup(replacement);

		char *rest = strtok(NULL, "");

		if (rest) {
			size_t rest_len = strlen(rest);
			actual =
			    realloc(actual, replacement_len + rest_len + 2);
			strcat(actual, " ");
			strcat(actual, rest);
		}

		free(tmp);

		return actual;
	} else {
		free(tmp);

		return copy;
	}
}
