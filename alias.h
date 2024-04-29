// alias.h

#ifndef ALIAS_H
#define ALIAS_H

#include "shell.h"

#define MAX_ALIASES 10

typedef struct {
	char *name;
	char *replacement;
} Alias;

extern Alias aliases[MAX_ALIASES];
extern int num_aliases;

void add_alias(const char *name, const char *replacement);
const char *find_alias(const char *name);
char *expand_alias(const char *command);

#endif 
