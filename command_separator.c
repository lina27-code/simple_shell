#include "main.h"

int execute_commands(char *commands, char *program_name)
{
    char *command;
    char *commands_copy;
    int status = 0;

    if (commands == NULL || commands[0] == '\0')
        return (0);

    /* Make a copy of the commands string */
    commands_copy = strdup(commands);
    if (commands_copy == NULL)
        return (-1);

    /* Parse and execute each command separated by ; */
    command = _strtok(commands_copy, ";");
    while (command != NULL)
    {
        /* Skip leading whitespace */
        while (*command == ' ' || *command == '\t')
            command++;

        if (command[0] != '\0')
        {
            /* Execute logical operators for this command segment */
            status = execute_logical_ops(command, program_name);
        }

        command = _strtok(NULL, ";");
    }

    free(commands_copy);
    return (status);
}
