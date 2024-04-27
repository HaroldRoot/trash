// shell.h

#ifndef SHELL_H
#define SHELL_H

// Constants
#define MAX_COMMAND_LENGTH 1024
#define PROMPT_FORMAT "[%s@%s %s]%c "

// Function declarations
char *get_current_directory();
char *get_username();
char *get_hostname();
void print_prompt();
char *trim_leading_space(char *str);
int is_builtin_command(char *command);
void handle_builtin_command(char *command);
void execute_command(char *command);

#endif				// SHELL_H
