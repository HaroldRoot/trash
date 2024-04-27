// builtin.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

CommandType type_of(const char *command)
{
	if (strncmp(command, "exit", strlen("exit")) == 0) {
		return BUILTIN_EXIT;
	} else if (strncmp(command, "cd", strlen("cd")) == 0) {
		int i = strlen("cd");
		while (command[i] != '\0') {
			if (command[i] != ' ') {
				return BUILTIN_CD_PATH;
			}
			i++;
		}
		return BUILTIN_CD_DEFAULT;
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
