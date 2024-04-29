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

char *expand_alias(const char *command)
{
	char *tmp_command = strdup(command);

	char *first_word = strtok(tmp_command, " \t\r\n");

	char *copy_command = strdup(command);

	const char *replacement = find_alias(first_word);

	if (replacement) {
		size_t replacement_len = strlen(replacement);

		char *actual_command = strdup(replacement);

		char *rest_of_command = strtok(NULL, "");

		if (rest_of_command) {
			size_t rest_len = strlen(rest_of_command);
			actual_command =
			    realloc(actual_command,
				    replacement_len + rest_len + 2);
			strcat(actual_command, " ");
			strcat(actual_command, rest_of_command);
		}

		free(tmp_command);

		return actual_command;
	} else {
		free(tmp_command);

		return copy_command;
	}
}
