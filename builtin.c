// builtin.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

void handle_builtin_command(char *command)
{
	if (strcmp(command, "exit") == 0) {
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
	}
	// Add other builtin commands here
}