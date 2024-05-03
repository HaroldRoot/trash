// redirect.h

#ifndef REDIRECT_H
#define REDIRECT_H

#include "shell.h"

void handle_redirections(char **argv, int *in_fd, int *out_fd, int *err_fd);
void redirect_stdout(char **argv, int i, int *out_fd);
void redirect_stderr(char **argv, int i, int *err_fd);
void redirect_stdout_append(char **argv, int i, int *out_fd);
void redirect_stderr_append(char **argv, int i, int *err_fd);
void redirect_stderr_to_stdout(char **argv, int i);
void redirect_input(char **argv, int i, int *in_fd);
void restore_std(int in_fd, int out_fd, int err_fd, int stdin_copy,
		 int stdout_copy, int stderr_copy);
void remove_argv(char **argv, int i);
void free_argv(char **argv);

#endif
