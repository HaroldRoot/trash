// utils.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

char *get_current_directory()
{
	char *cwd = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	if (cwd == NULL) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	if (getcwd(cwd, MAX_COMMAND_LENGTH) == NULL) {
		perror("getcwd() error");
		exit(EXIT_FAILURE);
	}
	// Replace user home directory path with ~
	struct passwd *pw = getpwuid(getuid());
	const char *home_dir = pw->pw_dir;
	size_t home_dir_len = strlen(home_dir);

	if (strncmp(cwd, home_dir, home_dir_len) == 0) {
		cwd[0] = '~';
		memmove(cwd + 1, cwd + home_dir_len,
			strlen(cwd + home_dir_len) + 1);
	}

	return cwd;
}

char *get_username()
{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw == NULL) {
		perror("getpwuid() error");
		exit(EXIT_FAILURE);
	}
	return pw->pw_name;
}

char *get_hostname()
{
	char *hostname = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
	if (hostname == NULL) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	if (gethostname(hostname, MAX_COMMAND_LENGTH) != 0) {
		perror("gethostname() error");
		exit(EXIT_FAILURE);
	}
	return hostname;
}

char *trim_leading_space(char *str)
{
	while (isspace((unsigned char)*str))
		str++;
	return str;
}
