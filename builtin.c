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

CmdType check_cd(char **argv)
{
	if (argv[1])
		return BUILTIN_CD_PATH;
	return BUILTIN_CD_DEFAULT;
}
