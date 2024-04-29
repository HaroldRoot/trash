// builtin.c

#include "shell.h"

void handle_builtin_command(const char *command, CommandType t)
{
	switch (t) {
	case (BUILTIN_EXIT):
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
		break;
	case (BUILTIN_CD_PATH):
		if (chdir(parse_path(command)) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_CD_DEFAULT):
		if (chdir(get_home_directory()) != 0)
			perror("cd failed");
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
