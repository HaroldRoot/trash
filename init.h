// init.h

#ifndef INIT_H
#define INIT_H

extern char startup_directory[PATH_MAX - 20];
extern char history_file_path[PATH_MAX];
extern char *username;
extern char *hostname;
extern char *current_directory;

void init();

#endif
