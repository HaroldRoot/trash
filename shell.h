// shell.h

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "alias.h"

// Macros
#define check_null(ptr) \
	do { \
		if (ptr == NULL) { \
			perror("memory allocation error"); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define exit_if(expression) \
	do { \
		if (expression) { \
			perror("error: " #expression); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

// Constants
#define MAX_COMMAND_LENGTH 1024
#define BUFSIZE 64
#define PROMPT_FORMAT "[%s@%s %s]%c "
#define MAX_FILES 20
#define MAX_FILENAME_LENGTH 20
extern const char *builtins[];
extern char startup_directory[PATH_MAX];
extern char history_file_path[PATH_MAX + 50];

// Enums
typedef enum {
	BUILTIN_EXIT,
	BUILTIN_HELP,
	BUILTIN_HISTORY,
	BUILTIN_CD_PATH,
	BUILTIN_CD_DEFAULT,
	BUILTIN_ALIAS,
	BUILTIN_UNALIAS,
	NOT_BUILTIN
} CmdType;

typedef enum {
	EXECUTE_SUCCESS,
	EXECUTE_FAILURE
} ExecuteResult;

// Function declarations
char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();
char *get_which(char *cmd);

void print_prompt();

char *read_input();

char *trim_leading_space(char *str);
char *parse_path(char *str);

void execute(char *cmd);
void handle_external(char *cmd);
char **parse(char *cmd);
ExecuteResult execute_external(char **argv);

CmdType type_of(char **argv);
CmdType check_cd(char **argv);

void handle_builtin(char **argv, CmdType t);

void print_help();
void print_logo();
int num_builtins();
void save_history(char *input);
void print_history();

#endif				// SHELL_H
