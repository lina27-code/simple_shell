#include "main.h"

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
        if (isatty(STDIN_FILENO))
            display_prompt();

        command = read_command();
        if (command == NULL)
        {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        last_exit_status = status;
        replace_variables(&command, last_exit_status);
        status = execute_commands(command, av[0]);
        free(command);
    }

    free_prev_dir();
    free_aliases();

    return (status);
}
