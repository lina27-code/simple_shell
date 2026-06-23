#include "shell.h"

/**
 * parse_command - splits a command into arguments
 * @command: the command string
 * @argv: array to store arguments
 *
 * Return: number of arguments
 */
int parse_command(char *command, char **argv)
{
    char *token;
    int i = 0;

    /* Skip leading whitespace */
    while (*command == ' ' || *command == '\t')
        command++;

    token = strtok(command, " \t");
    while (token != NULL && i < 127)
    {
        argv[i++] = token;
        token = strtok(NULL, " \t");
    }
    argv[i] = NULL;

    return (i);
}

/**
 * execute_command - executes a command
 * @command: the command to execute
 * @program_name: name of the program (for error messages)
 *
 * Return: exit status of the command
 */
int execute_command(char *command, char *program_name)
{
    pid_t pid;
    int status;
    int i;
    char *argv[128];
    int argc;
    char *full_path;

    /* Handle empty command (just newline or spaces) */
    if (command == NULL || command[0] == '\0')
        return (0);

    /* Check if command is only whitespace */
     i = 0;
    while (command[i] == ' ' || command[i] == '\t')
        i++;
    if (command[i] == '\0')
        return (0);

    /* Parse command into arguments */
    argc = parse_command(command, argv);
    if (argc == 0)
        return (0);

    /* Handle built-ins */
    if (is_builtin(argv))
    {
        execute_builtin(argv, &status);
        return (status);
    }

    /* Find command in PATH */
    full_path = find_in_path(argv[0]);
    if (full_path == NULL)
    {
        print_error(program_name, argv[0]);
        return (127);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return (1);
    }

    if (pid == 0)
    {
        /* Child process */
        if (execve(full_path, argv, environ) == -1)
        {
            free(full_path);
            print_error(program_name, argv[0]);
            exit(127);
        }
    }
    else
    {
        /* Parent process - wait for child */
        wait(&status);
        free(full_path);
    }

    return (WEXITSTATUS(status));
}
