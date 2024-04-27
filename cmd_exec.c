// cmd_exec.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

void execute_command(char *command)
{
	command = trim_leading_space(command);
	if (is_builtin_command(command)) {
		handle_builtin_command(command);
	} else {
		// Implement executing non-builtin commands here
		system(command);
	}
}
