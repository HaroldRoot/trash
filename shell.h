// shell.h

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <fcntl.h>
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
#include "redirect.h"
#include "init.h"

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
#define BUFSIZE 64
#define PROMPT_FORMAT "[%s@%s %s]%c "
#define MAX_FILES 20
#define MAX_FILENAME_LENGTH 20
#define HISTSIZE 1000

// Enums
typedef enum {
	EXECUTE_SUCCESS,
	EXECUTE_FAILURE
} ExecuteResult;

// Function pointer type definition
typedef void (*BuiltinHandler)(char **argv);

// Struct
typedef struct {
	const char *name;
	BuiltinHandler handler;
} BuiltinCmd;

// Function declarations
char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();
char *get_which(char *cmd);

void print_prompt();

char *read_input();
void process(char *input);

char *trim_leading_space(char *str);
char *parse_path(char *str);
void strip_quotes(char **word);

void execute(char **argv, char *actual);
char **tokenize(char *cmd);
char *detokenize(char **argv);
void handle_external(char **argv, char *cmd);
ExecuteResult execute_external(char **argv);

int handle_builtin(char **argv);
void exit_shell(char **argv);
void handle_cd(char **argv);
void handle_alias(char **argv);
void handle_unalias(char **argv);
void handle_history(char **argv);
void print_help(char **argv);

void print_logo();
int num_builtins();
void save_history(char *input);
void trim_history();
void print_history();

#endif				// SHELL_H
