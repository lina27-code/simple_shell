#include "main.h"

/**
 * free_shell_resources - frees all shell resources
 */
void free_shell_resources(void)
{
    free_prev_dir();
    free_aliases();
}

/**
 * shell_exit - exits the shell cleanly
 * @status: exit status
 */
void shell_exit(int status)
{
    free_shell_resources();
    exit(status);
}        
