// builtin.h

#ifndef BUILTIN_H
#define BUILTIN_H

typedef void (*BuiltinHandler)(char **argv);

typedef struct {
	const char *name;
	BuiltinHandler handler;
} BuiltinCmd;

extern BuiltinCmd builtins[];
int num_builtins();

typedef struct {
	pid_t pid;
	char *cmd;
} BgJob;

#define MAX_JOBS 10

extern BgJob bg_jobs[MAX_JOBS];
extern int num_bg_jobs;

int handle_builtin(char **argv);

void exit_shell(char **argv);

void print_help(char **argv);
void print_logo();

void handle_alias(char **argv);
void handle_unalias(char **argv);

void handle_cd(char **argv);

void handle_history(char **argv);
void save_history(char *input);
void print_history(int n);

void handle_jobs(char **argv);
void add_bg_job(pid_t pid, char *cmd);
void print_bg_jobs();
char *get_cmd_by_pid(pid_t pid);
void remove_bg_job(pid_t pid);
int is_bg_job(pid_t pid);

#endif
