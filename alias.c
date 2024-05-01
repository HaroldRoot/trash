// alias.c

#include "shell.h"

Alias *find_alias(const char *name)
{
	Alias *current = aliases_head;
	while (current != NULL) {
		if (strcmp(current->name, name) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

void add_alias(const char *name, const char *replacement)
{
	Alias *existing_alias = find_alias(name);
	if (existing_alias) {
		free(existing_alias->replacement);
		existing_alias->replacement = strdup(replacement);
	} else {
		// Add new alias
		Alias *new_alias = malloc(sizeof(Alias));
		new_alias->name = strdup(name);
		new_alias->replacement = strdup(replacement);
		new_alias->next = aliases_head;
		aliases_head = new_alias;
		num_aliases++;
	}
}

void remove_alias(const char *name)
{
	// This is a pointer to a pointer to the head of the list.
	Alias **current = &aliases_head;
	Alias *prev = NULL;

	// Loop through the list until we reach the end.
	while (*current) {
		// Dereference 'current' to get the actual alias entry.
		Alias *entry = *current;

		// Check if the current entry's name matches the one we're looking for.
		if (strcmp(entry->name, name) == 0) {
			// If 'prev' is not NULL, it means we're not at the head of the list.
			if (prev) {
				// Set 'prev's next pointer to skip over the entry we're removing.
				prev->next = entry->next;
			} else {
				// If we're at the head, update the head to the next element.
				aliases_head = entry->next;
			}
			// Free the memory allocated for the alias we're removing.
			free(entry->name);
			free(entry->replacement);
			free(entry);
			// Decrement the count of aliases.
			num_aliases--;
			return;
		}
		// Update 'prev' to the current entry before moving to the next.
		prev = entry;
		// Move to the next entry in the list.
		current = &entry->next;
	}
	// If we reach this point, the alias was not found.
	printf("Alias '%s' not found.\n", name);
}

void list_aliases()
{
	Alias *current = aliases_head;
	while (current != NULL) {
		printf("%s='%s'\n", current->name, current->replacement);
		current = current->next;
	}
}

char *expand_alias(const char *cmd)
{
	char *tmp = strdup(cmd);
	char *first_word = strtok(tmp, " \t\r\n");
	char *copy = strdup(cmd);

	Alias *alias_found = find_alias(first_word);

	if (alias_found) {
		const char *replacement = alias_found->replacement;
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
		free(copy);
		return actual;
	} else {
		free(tmp);
		return copy;
	}
}
