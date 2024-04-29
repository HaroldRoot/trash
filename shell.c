// shell.c

#include "shell.h"

char *username = NULL;
char *hostname = NULL;
char *current_directory = NULL;

void init_globals()
{
	username = get_username();
	hostname = get_hostname();
	current_directory = get_current_directory();
}

void free_globals()
{
	free(username);
	free(hostname);
	free(current_directory);
}

int main()
{
	if (isatty(STDIN_FILENO)) {
		// shell is running in interactive mode
		// display prompt and accept input from user
		init_globals();
		InputBuffer *input_buffer = new_input_buffer();

		while (1) {
			print_prompt();
			read_input(input_buffer);
			execute_command(input_buffer);
		}
	} else {
		// shell is running in non-interactive mode
		// execute commands from script or batch file
	}

	return 0;
}

InputBuffer *new_input_buffer()
{
	InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;

	return input_buffer;
}

void read_input(InputBuffer *input_buffer)
{
	ssize_t bytes_read =
	    getline(&(input_buffer->buffer), &(input_buffer->buffer_length),
		    stdin);

	if (bytes_read <= 0) {
		// Handle Ctrl+D or EOF
		printf("\nExiting trash...\n");
		free_globals();	// Free memory before exiting
		exit(EXIT_SUCCESS);
	}

	input_buffer->input_length = bytes_read - 1;
	input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer *input_buffer)
{
	free(input_buffer->buffer);
	free(input_buffer);
}
