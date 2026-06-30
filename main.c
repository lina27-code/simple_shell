#include "shell.h"

/**
 * main - entry point for the shell
 * @ac: argument count
 * @av: argument vector
 *
 * Return: 0 on success
 */
int main(int ac, char **av)
{
    char *command;
    int status = 0;
    (void)ac;

    while (1)
    {
        /* Display prompt only in interactive mode */
        if (isatty(STDIN_FILENO))
            display_prompt();

        /* Read command */
        command = read_command();
        if (command == NULL)
        {
            /* EOF (Ctrl+D) */
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        /* Execute commands with ; seperator handling */
        status = execute_commands(command, av[0]);

        free(command);
    }
    
    /* Clean up */
    free_prev_dir();
    return (status);
} 
