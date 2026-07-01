#include "shell.h"

/**
 * get_command_status - executes a single command and returns its status
 * @command: the command to execute
 * @program_name: name of the program (for error messages)
 *
 * Return: exit status of the command
 */
int get_command_status(char *command, char *program_name)
{
    int status;

    /* Skip leading whitespace */
    while (*command == ' ' || *command == '\t')
        command++;

    if (command[0] == '\0')
        return (0);

    /* Execute the command */
    status = execute_command(command, program_name);
    return (status);
}

/**
 * execute_logical_ops - executes commands with && and || operators
 * @commands: string containing commands with && and || operators
 * @program_name: name of the program (for error messages)
 *
 * Return: status of the last executed command
 */
int execute_logical_ops(char *commands, char *program_name)
{
    char *commands_copy;
    char *current;
    char *next_and;
    char *next_or;
    char *next_op;
    char cmd_copy[1024];
    int cmd_len;
    int status;
    int should_execute;

    if (commands == NULL || commands[0] == '\0')
        return (0);

    commands_copy = strdup(commands);
    if (commands_copy == NULL)
        return (-1);

    /* Check if there are any operators */
    if (strstr(commands_copy, "&&") == NULL && strstr(commands_copy, "||") == NULL)
    {
        status = get_command_status(commands_copy, program_name);
        free(commands_copy);
        return (status);
    }

    current = commands_copy;
    status = 0;
    should_execute = 1;

    while (*current != '\0')
    {
        /* Skip leading whitespace */
        while (*current == ' ' || *current == '\t')
            current++;

        if (*current == '\0')
            break;

        /* Find the next operator */
        next_and = strstr(current, "&&");
        next_or = strstr(current, "||");

        if (next_and == NULL && next_or == NULL)
        {
            /* No more operators - execute the remaining command */
            if (should_execute)
                status = get_command_status(current, program_name);
            break;
        }

        /* Determine which operator comes first */
        if (next_and != NULL && (next_or == NULL || next_and < next_or))
        {
            next_op = next_and;
            cmd_len = next_op - current;

            /* Copy the command without the operator */
            if (cmd_len >= (int)sizeof(cmd_copy))
                cmd_len = sizeof(cmd_copy) - 1;
            strncpy(cmd_copy, current, cmd_len);
            cmd_copy[cmd_len] = '\0';

            if (should_execute)
                status = get_command_status(cmd_copy, program_name);

            /* For &&: only execute next if status == 0 */
            if (status == 0)
                should_execute = 1;
            else
                should_execute = 0;

            current = next_op + 2;
        }
        else if (next_or != NULL && (next_and == NULL || next_or < next_and))
        {
            next_op = next_or;
            cmd_len = next_op - current;

            /* Copy the command without the operator */
            if (cmd_len >= (int)sizeof(cmd_copy))
                cmd_len = sizeof(cmd_copy) - 1;
            strncpy(cmd_copy, current, cmd_len);
            cmd_copy[cmd_len] = '\0';

            if (should_execute)
                status = get_command_status(cmd_copy, program_name);

            /* For ||: only execute next if status != 0 */
            if (status != 0)
                should_execute = 1;
            else
                should_execute = 0;

            current = next_op + 2;
        }
        else
        {
            /* Should not happen */
            break;
        }
    }

    free(commands_copy);
    return (status);
}
