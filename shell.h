// shell.h

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "alias.h"
#include "ansicode.h"
#include "builtin.h"
#include "execute.h"
#include "init.h"
#include "prompt.h"
#include "redirect.h"
#include "tokenize.h"
#include "utils.h"

// Macros
#define check_null(ptr) \
	do { \
		if (ptr == NULL) { \
			perror("memory allocation error"); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define exit_if(cond) \
	if ((cond)) { \
		perror("error: " #cond); \
		exit(EXIT_FAILURE); \
	} \

// Global constants and variables
#define MAX_WORDS 128
#define MAX_COMMAND_LENGTH 1024
#define MAX_CMD 10
#define MAX_ARGC 10
#define BUFSIZE 64
#define MAX_FILES 20
#define MAX_FILENAME_LENGTH 256
#define HISTSIZE 1000

// Function declarations
void process(char *input);
char* replace_env_vars(char* arg);

#endif				// SHELL_H
