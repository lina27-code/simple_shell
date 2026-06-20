#include "shell.h"

/**
 * free_command - frees a command string
 * @command: command to free
 */
void free_command(char *command)
{
    if (command != NULL)
        free(command);
}
