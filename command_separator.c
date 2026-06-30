#include "shell.h"

/**
 * execute_commands - executes multiple commands separated by ;
 * @commands: string containing commands separated by ;
 * @program_name: name of the program (for error messages)
 *
 * Return: status of the last executed command
 */
int execute_commands(char *commands, char *program_name)
{
    char *command;
    char *commands_copy;
    int status = 0;

    if (commands == NULL || commands[0] == '\0')
        return (0);

    /* Duplicate the commands string to avoid modifying the original */
    commands_copy = strdup(commands);
    if (commands_copy == NULL)
        return (-1);

    /* Tokenize by ; */
    command = _strtok(commands_copy, ";");
    while (command != NULL)
    {
        /* Skip leading whitespace */
        while (*command == ' ' || *command == '\t')
            command++;

        /* Execute the command if not empty */
        if (command[0] != '\0')
        {
            /* Execute the command */
            status = execute_command(command, program_name);
        }

        /* Get next command */
        command = _strtok(NULL, ";");
    }

    free(commands_copy);
    return (status);
}
