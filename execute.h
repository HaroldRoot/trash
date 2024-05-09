// execute.h

#ifndef EXECUTE_H
#define EXECUTE_H

typedef enum {
	EXECUTE_SUCCESS,
	EXECUTE_FAILURE
} ExecuteResult;

void execute(char *cmd);
void handle_external(char **argv, char *cmd, int run_in_background);
ExecuteResult execute_external(char **argv, int run_in_background);

#endif
