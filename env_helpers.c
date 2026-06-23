#include "shell.h"

/**
 * _getenv - gets the value of an environment variable
 * @name: name of the variable
 *
 * Return: pointer to the value, or NULL if not found
 */
char *_getenv(const char *name)
{
    char **env = environ;
    int len;

    if (name == NULL)
        return (NULL);

    len = strlen(name);
    while (*env != NULL)
    {
        if (strncmp(*env, name, len) == 0 && (*env)[len] == '=')
            return ((*env) + len + 1);
        env++;
    }
    return (NULL);
}

/**
 * _setenv - sets or updates an environment variable
 * @name: name of the variable
 * @value: value to set
 * @overwrite: 1 to overwrite existing, 0 to ignore if exists
 *
 * Return: 0 on success, -1 on error
 */
int _setenv(const char *name, const char *value, int overwrite)
{
    char **env = environ;
    char *new_entry;
    int name_len, value_len;
    int found = 0;

    if (name == NULL || value == NULL || name[0] == '\0')
    {
        write(STDERR_FILENO, "setenv: invalid arguments\n", 26);
        return (-1);
    }

    /* Check if variable already exists */
    while (*env != NULL)
    {
        int len = strlen(name);
        if (strncmp(*env, name, len) == 0 && (*env)[len] == '=')
        {
            found = 1;
            if (!overwrite)
                return (0);
            break;
        }
        env++;
    }

    /* Allocate memory for new entry */
    name_len = strlen(name);
    value_len = strlen(value);
    new_entry = malloc(name_len + value_len + 2);
    if (new_entry == NULL)
    {
        write(STDERR_FILENO, "setenv: memory allocation failed\n", 34);
        return (-1);
    }

    /* Create the entry: "NAME=VALUE" */
    strcpy(new_entry, name);
    strcat(new_entry, "=");
    strcat(new_entry, value);

    /* If variable exists, replace it */
    if (found)
    {
        *env = new_entry;
    }
    else
    {
        /* We need to reallocate environ to add a new entry */
        /* This is complex; for simplicity, we won't fully implement it */
        /* In a real shell, you'd need to manage environ memory */
        free(new_entry);
        write(STDERR_FILENO, "setenv: not fully implemented\n", 30);
        return (-1);
    }

    return (0);
}

/**
 * _unsetenv - removes an environment variable
 * @name: name of the variable to remove
 *
 * Return: 0 on success, -1 on error
 */
int _unsetenv(const char *name)
{
    char **env = environ;
    char **next_env;
    int len;

    if (name == NULL || name[0] == '\0')
    {
        write(STDERR_FILENO, "unsetenv: invalid argument\n", 27);
        return (-1);
    }

    len = strlen(name);
    while (*env != NULL)
    {
        if (strncmp(*env, name, len) == 0 && (*env)[len] == '=')
        {
            /* Found the variable - shift remaining entries */
            next_env = env;
            while (*next_env != NULL)
            {
                *next_env = *(next_env + 1);
                next_env++;
            }
            return (0);
        }
        env++;
    }
    return (0); /* Variable not found - success anyway */
}
