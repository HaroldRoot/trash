// builtin.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

CommandType type_of(const char *command)
{
	if (strncmp(command, "exit", 4) == 0) {
		return check_exit(command);
	} else if (strncmp(command, "cd", 2) == 0) {
		return check_cd(command);
	}
	// Add other builtin commands here
	return NOT_BUILTIN;
}

void handle_builtin_command(const char *command, CommandType t)
{
	switch (t) {
	case (BUILTIN_EXIT):
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
		break;
	case (BUILTIN_CD_PATH):
		printf("cd path\n");
		break;
	case (BUILTIN_CD_DEFAULT):
		printf("cd home\n");
		break;
	default:
		break;
		// Add other builtin commands here
	}
}

CommandType check_exit(const char *str)
{
	const char *exit_str = "exit";
	while (*exit_str != '\0' && *str != '\0' && *str == *exit_str) {
		str++;
		exit_str++;
	}

	if (*exit_str == '\0' && (*str == '\0' || isspace((unsigned char)*str))) {
		return BUILTIN_EXIT;
	} else {
		return NOT_BUILTIN;
	}
}

CommandType check_cd(const char *str)
{
	if (str[2] != '\0' && str[2] != ' ') {
		return NOT_BUILTIN;
	}

	str += 2;

	while (*str != '\0' && isspace((unsigned char)*str)) {
		str++;
	}

	if (*str == '\0' || isspace((unsigned char)*str)) {
		return BUILTIN_CD_DEFAULT;
	}

	return BUILTIN_CD_PATH;
}
