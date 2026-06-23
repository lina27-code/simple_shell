#include "shell.h"

/**
 * _strtok - custom implementation of strtok
 * @str: string to tokenize
 * @delim: delimiter characters
 *
 * Return: pointer to next token, or NULL if no more tokens
 */
char *_strtok(char *str, const char *delim)
{
    static char *next_token = NULL;
    char *token_start;
    int i;
    int is_delim;

    /* If a new string is provided, start with it */
    if (str != NULL)
        next_token = str;

    if (next_token == NULL || *next_token == '\0')
        return (NULL);

    /* Skip leading delimiters */
    while (*next_token != '\0')
    {
        is_delim = 0;
        for (i = 0; delim[i] != '\0'; i++)
        {
            if (*next_token == delim[i])
            {
                is_delim = 1;
                break;
            }
        }
        if (!is_delim)
            break;
        next_token++;
    }

    if (*next_token == '\0')
        return (NULL);

    token_start = next_token;

    /* Find end of token */
    while (*next_token != '\0')
    {
        is_delim = 0;
        for (i = 0; delim[i] != '\0'; i++)
        {
            if (*next_token == delim[i])
            {
                is_delim = 1;
                break;
            }
        }
        if (is_delim)
            break;
        next_token++;
    }

    if (*next_token == '\0')
    {
        next_token = NULL;
    }
    else
    {
        *next_token = '\0';
        next_token++;
    }

    return (token_start);
}
