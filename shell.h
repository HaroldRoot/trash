// shell.h

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define PROMPT_FORMAT "[%s@%s %s]%c "

// Enums
typedef enum {
	BUILTIN_EXIT,
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

// Structs
typedef struct {
	char *buffer;
	size_t buffer_length;
	ssize_t input_length;
} InputBuffer;

// Function declarations
char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();
char *get_which(char *cmd);

void print_prompt();

InputBuffer *new_input_buffer();
void read_input(InputBuffer *input_buffer);
void close_input_buffer(InputBuffer *input_buffer);

char *trim_leading_space(char *str);
char *parse_path(char *str);

void execute(InputBuffer *input_buffer);
void handle_external(char *cmd);
char **parse(char *cmd);
ExecuteResult execute_external(char **argv);

CmdType type_of(char **argv);
CmdType check_cd(char **argv);

void handle_builtin(char **argv, CmdType t);

#endif				// SHELL_H
