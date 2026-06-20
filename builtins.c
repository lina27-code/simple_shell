#include "shell.h"

/**
 * print_env - prints the current environment
 *
 * Return: 0 on success
 */
int print_env(void)
{
    char **env = environ;

    while (*env != NULL)
    {
        write(STDOUT_FILENO, *env, strlen(*env));
        write(STDOUT_FILENO, "\n", 1);
        env++;
    }
    return (0);
}

/**
 * is_builtin - checks if a command is a built-in
 * @argv: command arguments
 *
 * Return: 1 if built-in, 0 otherwise
 */
int is_builtin(char **argv)
{
    if (argv == NULL || argv[0] == NULL)
        return (0);

    if (strcmp(argv[0], "exit") == 0)
        return (1);
    if (strcmp(argv[0], "env") == 0)
        return (1);

    return (0);
}

/**
 * execute_builtin - executes a built-in command
 * @argv: command arguments
 * @status: pointer to exit status
 *
 * Return: 1 if command was handled, 0 otherwise
 */
int execute_builtin(char **argv, int *status)
{
    if (argv == NULL || argv[0] == NULL)
        return (0);

    if (strcmp(argv[0], "exit") == 0)
        exit(*status);

    if (strcmp(argv[0], "env") == 0)
    {
        *status = print_env();
        return (1);
    }

    return (0);
}
