#include "shell.h"

/**
 * print_error - prints an error message
 * @program_name: name of the program
 * @command: command that failed
 */

void print_error(char *program_name, char *command)
{
    write(STDERR_FILENO, program_name, strlen(program_name));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, command, strlen(command));
    write(STDERR_FILENO, "No such file or directory\n", 27);
    
}
