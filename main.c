#include "shell.h"

int shell_exiting = 0;  /* Global flag to prevent double free */

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

    /* Clean up - only once at the very end */
    free_prev_dir();
    free_aliases();
    return (status);
}        
