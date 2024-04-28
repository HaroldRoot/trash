// shell.h

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Constants
#define MAX_COMMAND_LENGTH 1024
#define PROMPT_FORMAT "[%s@%s %s]%c "

// Enums
typedef enum {
	BUILTIN_EXIT,
	BUILTIN_CD_PATH,
	BUILTIN_CD_DEFAULT,
	NOT_BUILTIN
} CommandType;

// Function declarations
char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();

void print_prompt();

char *trim_leading_space(char *str);
char *extract_second_word(const char *str);
char *parse_path(const char *str);

void execute_command(char *command);

CommandType type_of(const char *command);
CommandType check_exit(const char *command);
CommandType check_cd(const char *command);

void handle_builtin_command(const char *command, CommandType t);

#endif				// SHELL_H
