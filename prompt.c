 #include "shell.h"

/**
 * display_prompt - displays the shell prompt
 */
void display_prompt(void)
{
    write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

/**
 * read_command - reads a command from stdin using getline
 *
 * Return: pointer to the command string, or NULL on EOF/error
 */
char *read_command(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    nread = getline(&line, &len, stdin);
    if (nread == -1)
    {
        free(line);
        return (NULL);
    }

    /* Remove newline character */
    if (line[nread - 1] == '\n')
        line[nread - 1] = '\0';

    return (line);
}
