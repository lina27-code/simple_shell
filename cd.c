#include "main.h"

/* Global variable to store previous directory for cd - */
char *prev_dir = NULL;

/**
 * _cd - changes the current directory
 * @argv: command arguments (argv[1] is the target directory)
 *
 * Return: 0 on success, -1 on failure
 */
int _cd(char **argv)
{
    char *target_dir;
    char *home_dir;
    char *old_pwd;
    char cwd[1024];

    if (argv[1] == NULL)
    {
        home_dir = _getenv("HOME");
        if (home_dir == NULL)
        {
            write(STDERR_FILENO, "cd: HOME not set\n", 17);
            return (-1);
        }
        target_dir = home_dir;
    }
    else if (strcmp(argv[1], "-") == 0)
    {
        if (prev_dir == NULL)
        {
            write(STDERR_FILENO, "cd: OLDPWD not set\n", 19);
            return (-1);
        }
        target_dir = prev_dir;
    }
    else
    {
        target_dir = argv[1];
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return (-1);
    }

    old_pwd = strdup(cwd);
    if (old_pwd == NULL)
        return (-1);
    
    /* Change directory */
    if (chdir(target_dir) != 0)
    {
        perror("cd");
        free(old_pwd);
        return (-1);
    }

    /* Update previous directory */
    if (prev_dir != NULL)
        free(prev_dir);
    prev_dir = old_pwd;

    /* Update PWD environment variable */
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        _setenv("PWD", cwd, 1);
    }

    return (0);
}

/**
 * free_prev_dir - frees the prev_dir global variable
 * Called when the shell exits
 */
void free_prev_dir(void)
{
    if (prev_dir != NULL)
    {
        free(prev_dir);
        prev_dir = NULL;
    }
}
