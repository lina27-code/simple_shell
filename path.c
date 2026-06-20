#include "shell.h"

/**
 * find_in_path - searches for a command in PATH
 * @command: the command to find
 *
 * Return: full path if found, NULL if not
 */
char *find_in_path(char *command)
{
    char *path, *path_copy, *dir;
    char *full_path;
    struct stat st;

    if (command == NULL)
        return (NULL);

    /* If command already contains a slash, check if it's executable */
    if (strchr(command, '/') != NULL)
    {
        if (stat(command, &st) == 0 && (st.st_mode & S_IXUSR))
            return (strdup(command));
        return (NULL);
    }

    /* Get PATH environment variable */
    path = getenv("PATH");
    if (path == NULL)
        return (NULL);

    path_copy = strdup(path);
    if (path_copy == NULL)
        return (NULL);

    /* Search through each directory in PATH */
    dir = strtok(path_copy, ":");
    while (dir != NULL)
    {
        full_path = malloc(strlen(dir) + strlen(command) + 2);
        if (full_path == NULL)
        {
            free(path_copy);
            return (NULL);
        }

        sprintf(full_path, "%s/%s", dir, command);
        if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR))
        {
            free(path_copy);
            return (full_path);
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return (NULL);
}
