// utils.h

#ifndef UTILS_H
#define UTILS_H

char *get_current_directory();
char *get_home_directory();
char *get_username();
char *get_hostname();
char *get_which(char *cmd);
char *parse_path(char *path);
void strip_quotes(char **word);
char *trim_leading_spaces(char *str);
char *trim_spaces(const char *input);

#endif
