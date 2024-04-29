// builtin.c

#include "shell.h"

void handle_builtin(const char *cmd, CmdType t)
{
	switch (t) {
	case (BUILTIN_EXIT):
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
		break;
	case (BUILTIN_CD_PATH):
		if (chdir(parse_path(cmd)) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_CD_DEFAULT):
		if (chdir(get_home_directory()) != 0)
			perror("cd failed");
		break;
	default:
		break;
		// Add other builtin cmds here
	}
}

CmdType check_exit(const char *str)
{
	if (str[4] == ' ' || str[4] == '\0')
		return BUILTIN_EXIT;
	return NOT_BUILTIN;
}

CmdType check_cd(const char *str)
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
