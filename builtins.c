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
    if (strcmp(argv[0], "setenv") == 0)
        return (1);
    if (strcmp(argv[0], "unsetenv") == 0)
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
     int exit_status;

    if (argv == NULL || argv[0] == NULL)
        return (0);

    /* Handle exit with optional status */
    if (strcmp(argv[0], "exit") == 0)
    {
        if (argv[1] != NULL)
        {
            if (is_number(argv[1]))
            {
                exit_status = _atoi(argv[1]);
                free_prev_dir(); /* Clean up */
                exit(exit_status);
            }
            else
            {
                write(STDERR_FILENO, "./hsh: exit: Illegal number: ", 29);
                write(STDERR_FILENO, argv[1], strlen(argv[1]));
                write(STDERR_FILENO, "\n", 1);
                *status = 2;
                return (1);
            }
        }
        else
        {
            free_prev_dir(); /* Clean up */
            exit(*status);
        }
    }

    /* Handle env */
    if (strcmp(argv[0], "env") == 0)
    {
        *status = print_env();
        return (1);
    }

    /* Handle setenv */
    if (strcmp(argv[0], "setenv") == 0)
    {
        if (argv[1] == NULL || argv[2] == NULL)
        {
            write(STDERR_FILENO, "setenv: Usage: setenv VARIABLE VALUE\n", 37);
            *status = 1;
            return (1);
        }
        *status = _setenv(argv[1], argv[2], 1);
        return (1);
    }

    /* Handle unsetenv */
    if (strcmp(argv[0], "unsetenv") == 0)
    {
        if (argv[1] == NULL)
        {
            write(STDERR_FILENO, "unsetenv: Usage: unsetenv VARIABLE\n", 35);
            *status = 1;
            return (1);
        }
        *status = _unsetenv(argv[1]);
        return (1);
    }

    /* Handle cd */
    if (strcmp(argv[0], "cd") == 0)
    {
        *status = _cd(argv);
        return (1);
    }

    return (0);
}
             
