// init.h

#ifndef INIT_H
#define INIT_H

extern char startup_directory[PATH_MAX - 20];
extern char history_file_path[PATH_MAX];
extern char *username;
extern char *hostname;
extern char *current_directory;

char **command_completion(const char *text, int start, int end);
char *command_generator(const char *text, int state);
int is_executable(const char *path);
char **get_commands();
void initialize_readline();
void sigchld_handler(int signum);
void init();

#endif
