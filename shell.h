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
#include "redirect.h"
#include "init.h"
#include "ansicode.h"

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
#define PROMPT_FORMAT "[" BOLD BLUE "%s@%s " RESET BOLD GREEN "%s" RESET "]%c "
#define MAX_FILES 20
#define MAX_FILENAME_LENGTH 20
#define HISTSIZE 1000
#define MAX_JOBS 10

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

extern BuiltinCmd builtins[];

typedef struct {
	pid_t pid;
	char *cmd;
} BgJob;

extern BgJob bg_jobs[MAX_JOBS];
extern int num_bg_jobs;

// Function declarations
char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();
char *get_which(char *cmd);

char *prompt();

void process(char *input);

char *trim_leading_spaces(char *str);
char *trim_spaces(const char *input);
char *parse_path(char *str);
void strip_quotes(char **word);

void execute(char *cmd);
char **tokenize(char *cmd);
char *detokenize(char **argv);
void handle_external(char **argv, char *cmd,int run_in_background);
ExecuteResult execute_external(char **argv,int run_in_background);

int handle_builtin(char **argv);
int num_builtins();
void exit_shell(char **argv);
void handle_cd(char **argv);
void handle_alias(char **argv);
void handle_unalias(char **argv);
void handle_history(char **argv);
void save_history(char *input);
void print_history();
void print_help(char **argv);
void print_logo();
void handle_jobs(char **argv);
void add_bg_job(pid_t pid, char *cmd);
void print_bg_jobs();
char *get_cmd_by_pid(pid_t pid);
void remove_bg_job(pid_t pid);

#endif				// SHELL_H
