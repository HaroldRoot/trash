// alias.h

#ifndef ALIAS_H
#define ALIAS_H

#include "shell.h"

typedef struct Alias {
	char *name;
	char *replacement;
	struct Alias *next;
} Alias;

extern Alias *aliases_head;
extern int num_aliases;

Alias *find_alias(const char *name);
void add_alias(const char *name, const char *replacement);
void remove_alias(const char *name);
void list_aliases();
char *expand_alias(const char *cmd);

#endif 
