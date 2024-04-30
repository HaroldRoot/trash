// builtin.c

#include "shell.h"

void handle_builtin(char **argv, CmdType t)
{
	switch (t) {
	case (BUILTIN_EXIT):
		printf("Exiting trash...\n");
		exit(EXIT_SUCCESS);
		break;
	case (BUILTIN_CD_PATH):
		if (chdir(parse_path(argv[1])) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_CD_DEFAULT):
		if (chdir(get_home_directory()) != 0)
			perror("cd failed");
		break;
	case (BUILTIN_ALIAS):
		if (argv[1] == NULL) {
			list_aliases();
		} else if (argv[1] != NULL && argv[2] != NULL) {
			add_alias(argv[1], argv[2]);
		} else {
			fprintf(stderr, "Usage: alias <alias> <replacement>\n");
		}
		break;
	case (BUILTIN_UNALIAS):
		if (argv[1] != NULL) {
			remove_alias(argv[1]);
		} else {
			fprintf(stderr, "Usage: unalias <alias>\n");
		}
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
