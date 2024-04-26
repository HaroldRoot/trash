// prompt.c

#include <stdio.h>
#include <unistd.h>
#include "shell.h"

void print_prompt()
{
	char prompt_symbol = '$';
	if (geteuid() == 0) {
		prompt_symbol = '#';
	}
	printf(PROMPT_FORMAT, get_username(), get_hostname(),
	       get_current_directory(), prompt_symbol);
}
