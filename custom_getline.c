#include "shell.h"

#define READ_BUFFER_SIZE 1024

static char buffer[READ_BUFFER_SIZE];
static size_t buffer_pos = 0;
static ssize_t buffer_size = 0;

/**
 * _getline - custom implementation of getline
 * @lineptr: pointer to buffer to store the line
 * @n: pointer to size of the buffer
 * @stream: file stream to read from (unused)
 *
 * Return: number of characters read, or -1 on EOF/error
 */
ssize_t _getline(char **lineptr, size_t *n, FILE *stream)
{
    size_t line_len = 0;
    char *line = NULL;
    char *new_ptr;
    char c;

    (void)stream;

    /* If no buffer is allocated, allocate one */
    if (*lineptr == NULL || *n == 0)
    {
        *n = READ_BUFFER_SIZE;
        *lineptr = malloc(sizeof(char) * (*n));
        if (*lineptr == NULL)
            return (-1);
    }

    line = *lineptr;

    while (1)
    {
        /* If buffer is empty, refill it */
        if (buffer_pos >= (size_t)buffer_size)
        {
            buffer_size = read(STDIN_FILENO, buffer, READ_BUFFER_SIZE);
            buffer_pos = 0;

            if (buffer_size <= 0)
            {
                if (line_len == 0)
                    return (-1);
                break;
            }
        }

        /* Get next character from buffer */
        c = buffer[buffer_pos++];

        /* Store character in the line */
        if (line_len + 1 >= *n)
        {
            *n *= 2;
            new_ptr = realloc(*lineptr, *n);
            if (new_ptr == NULL)
                return (-1);
            *lineptr = new_ptr;
            line = *lineptr;
        }

        line[line_len++] = c;

        /* If newline, stop */
        if (c == '\n')
            break;
    }

    line[line_len] = '\0';
    return (line_len);
}

/**
 * _free_getline_buffer - frees the static buffer used by _getline
 * This is called when the shell exits to prevent memory leak reports
 */
void _free_getline_buffer(void)
{
    /* The static buffer doesn't need to be freed,
     * but we can clear it to satisfy valgrind */
    /* Nothing to free - buffer is static */
}
