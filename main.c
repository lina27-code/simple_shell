#include "shell.h"

int main(int ac, char **av)
{
    char *command;
    int status = 0;
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    /* Task 16: File input mode */
    if (ac == 2)
    {
        file = fopen(av[1], "r");
        if (file == NULL)
        {
            perror("fopen");
            return (1);
        }

        while ((nread = getline(&line, &len, file)) != -1)
        {
            /* Remove newline */
            if (line[nread - 1] == '\n')
                line[nread - 1] = '\0';

            /* Skip empty lines */
            if (line[0] != '\0')
            {
                last_exit_status = status;
                replace_variables(&line, last_exit_status);
                status = execute_commands(line, av[0]);
            }

            /* Free the line after each iteration */
            free(line);
            line = NULL;
            len = 0;
        }

        free(line);
        fclose(file);
        free_prev_dir();
        free_aliases();
        return (status);
    }

    /* Interactive mode */
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
