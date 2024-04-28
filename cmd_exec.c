// cmd_exec.c

#include "shell.h"

void execute_command(char *command)
{
	command = trim_leading_space(command);
	CommandType t = type_of(command);
	if (t == NOT_BUILTIN) {
		system(command);
	} else {
		handle_builtin_command(command, t);
	}
}
