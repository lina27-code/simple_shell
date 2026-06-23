#include "shell.h"

/**
 * is_number - checks if a string is a valid number
 * @s: string to check
 *
 * Return: 1 if all characters are digits, 0 otherwise
 */
int is_number(char *s)
{
    int i = 0;

    if (s == NULL || s[0] == '\0')
        return (0);

    /* Allow optional leading minus sign */
    if (s[0] == '-')
        i = 1;

    while (s[i] != '\0')
    {
        if (s[i] < '0' || s[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

/**
 * _atoi - custom implementation of atoi
 * @s: string to convert
 *
 * Return: integer value of the string
 */
int _atoi(char *s)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    if (s == NULL)
        return (0);

    /* Handle leading minus sign */
    if (s[0] == '-')
    {
        sign = -1;
        i = 1;
    }

    while (s[i] != '\0')
    {
        if (s[i] < '0' || s[i] > '9')
            break;
        result = result * 10 + (s[i] - '0');
        i++;
    }

    return (result * sign);
}
