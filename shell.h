// shell.h

#ifndef SHELL_H
#define SHELL_H

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
char *get_username();
char *get_hostname();
void print_prompt();
char *trim_leading_space(char *str);
CommandType type_of(const char *command);
void handle_builtin_command(const char *command, CommandType t);
void execute_command(char *command);

#endif				// SHELL_H
