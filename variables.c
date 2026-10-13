#include "main.h"

/* Global variables for variable replacement */
int last_exit_status = 0;
pid_t shell_pid = 0;

/**
 * replace_variables - replaces $? and $$ in a command string
 * @command: pointer to the command string
 * @status: current exit status
 *
 * Return: 1 if variables were replaced, 0 otherwise
 */
int replace_variables(char **command, int status)
{
    char *cmd;
    char *new_cmd;
    char *pos;
    char pid_str[16];
    char status_str[16];

    if (command == NULL || *command == NULL)
        return (0);

    cmd = *command;

    /* Check if there are any variables to replace */
    if (strstr(cmd, "$?") == NULL && strstr(cmd, "$$") == NULL)
        return (0);

    /* Convert PID and status to strings */
    shell_pid = getpid();
    sprintf(pid_str, "%d", shell_pid);
    sprintf(status_str, "%d", status);

    /* Allocate memory for new command */
    new_cmd = malloc(strlen(cmd) + 16 + 1);
    if (new_cmd == NULL)
        return (0);

    new_cmd[0] = '\0';
    pos = cmd;

    while (*pos != '\0')
    {
        /* Look for $? or $$ */
        if (*pos == '$' && *(pos + 1) == '?')
        {
            /* Found $? - replace with exit status */
            strcat(new_cmd, status_str);
            pos += 2;
        }
        else if (*pos == '$' && *(pos + 1) == '$')
        {
            /* Found $$ - replace with PID */
            strcat(new_cmd, pid_str);
            pos += 2;
        }
        else
        {
            /* Copy character as is */
            char temp[2];
            temp[0] = *pos;
            temp[1] = '\0';
            strcat(new_cmd, temp);
            pos++;
        }
    }

    free(*command);
    *command = new_cmd;
    return (1);
}
