#include "main.h"

/* Head of aliases linked list */
static alias_t *alias_head = NULL;

/**
 * add_alias - adds or updates an alias
 * @name: alias name
 * @value: alias value
 *
 * Return: 0 on success, -1 on failure
 */
int add_alias(char *name, char *value)
{
    alias_t *current;
    alias_t *new_alias;

    if (name == NULL || value == NULL)
        return (-1);

    /* Check if alias already exists */
    current = alias_head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            /* Update existing alias */
            free(current->value);
            current->value = strdup(value);
            if (current->value == NULL)
                return (-1);
            return (0);
        }
        current = current->next;
    }

    /* Create new alias */
    new_alias = malloc(sizeof(alias_t));
    if (new_alias == NULL)
        return (-1);

    new_alias->name = strdup(name);
    if (new_alias->name == NULL)
    {
        free(new_alias);
        return (-1);
    }

    new_alias->value = strdup(value);
    if (new_alias->value == NULL)
    {
        free(new_alias->name);
        free(new_alias);
        return (-1);
    }

    new_alias->next = alias_head;
    alias_head = new_alias;

    return (0);
 }

/**
 * get_alias - gets the value of an alias
 * @name: alias name
 *
 * Return: pointer to value, or NULL if not found
 */
char *get_alias(char *name)
{
    alias_t *current = alias_head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return (current->value);
        current = current->next;
    }
    return (NULL);
}

/**
 * print_aliases - prints all aliases in name='value' format
 */
void print_aliases(void)
{
    alias_t *current = alias_head;

    while (current != NULL)
    {
        write(STDOUT_FILENO, current->name, strlen(current->name));
        write(STDOUT_FILENO, "='", 2);
        write(STDOUT_FILENO, current->value, strlen(current->value));
        write(STDOUT_FILENO, "'\n", 2);
        current = current->next;
    }
}

/**
 * print_single_alias - prints a single alias
 * @name: alias name
 *
 * Return: 0 if found, -1 if not
 */
int print_single_alias(char *name)
{
    char *value = get_alias(name);

    if (value == NULL)
        return (-1);

    write(STDOUT_FILENO, name, strlen(name));
    write(STDOUT_FILENO, "='", 2);
    write(STDOUT_FILENO, value, strlen(value));
    write(STDOUT_FILENO, "'\n", 2);
    return (0);
}

/**
 * free_aliases - frees all aliases
 */
void free_aliases(void)
{
    alias_t *current = alias_head;
    alias_t *next;
    static int already_freed = 0;
    
    if (already_freed || alias_head == NULL)
        return;
    already_freed = 1;

    while (current != NULL)
    {
        next = current->next;
        free(current->name);
        free(current->value);
        free(current);
        current = next;
    }
    alias_head = NULL;
}

/**
 * expand_aliases - replaces alias names with their values in a command
 * @command: pointer to the command string
 *
 * Return: 1 if alias was expanded, 0 otherwise
 */
int expand_aliases(char **command)
{
    char *first_word;
    char *alias_value;
    char *new_command;
    char *space_pos;

    if (command == NULL || *command == NULL)
        return (0);

    /* Get the first word (command name) */
    first_word = *command;

    /* Find space or tab */
    space_pos = first_word;
    while (*space_pos != '\0' && *space_pos != ' ' && *space_pos != '\t')
        space_pos++;

    /* Temporarily terminate the string at the space to get the command */
    if (*space_pos != '\0')
    {
        *space_pos = '\0';
        alias_value = get_alias(first_word);
        *space_pos = ' ';
    }
    else
    {
        alias_value = get_alias(first_word);
    }

    if (alias_value == NULL)
        return (0);

    /* Replace command with alias value */
    if (*space_pos != '\0')
    {
        /* Command has arguments - preserve them */
        int args_len = strlen(space_pos);
        int alias_len = strlen(alias_value);

        new_command = malloc(alias_len + args_len + 2);
        if (new_command == NULL)
            return (0);

        strcpy(new_command, alias_value);
        if (args_len > 0)
        {
            strcat(new_command, " ");
            strcat(new_command, space_pos);
        }
    }
    else
    {
        /* Command only (no arguments) */
        new_command = strdup(alias_value);
        if (new_command == NULL)
            return (0);
    }

    free(*command);
    *command = new_command;
    return (1);
}

/**
 * alias_builtin - handles the alias built-in command
 * @argv: command arguments
 *
 * Return: 0 on success, -1 on failure
 */

 int alias_builtin(char **argv)
{
    int i;
    int status;
    int total_len;
    char *joined_args;
    char *current;
    char *next_space;
    char *eq_pos;
    char token[256];
    int token_len;
    int in_quotes;
    char *name;
    char *value;

    status = 0;
    total_len = 0;

    if (argv[1] == NULL)
    {
        print_aliases();
        return (0);
    }

    /* Calculate total length for joined arguments */
    for (i = 1; argv[i] != NULL; i++)
    {
        total_len += strlen(argv[i]) + 1;
    }

    joined_args = malloc(total_len + 1);
    if (joined_args == NULL)
        return (-1);

    joined_args[0] = '\0';
    for (i = 1; argv[i] != NULL; i++)
    {
        strcat(joined_args, argv[i]);
        if (argv[i + 1] != NULL)
            strcat(joined_args, " ");
    }

    /* Parse the joined string for aliases */
    current = joined_args;

    while (*current != '\0')
    {
        /* Skip leading spaces */
        while (*current == ' ')
            current++;

        if (*current == '\0')
            break;

        /* Find the next space that's not inside quotes */
        in_quotes = 0;
        next_space = current;
        while (*next_space != '\0')
        {
            if (*next_space == '\'' || *next_space == '"')
                in_quotes = !in_quotes;
            else if (*next_space == ' ' && !in_quotes)
                break;
            next_space++;
        }

        /* Copy this token */
        token_len = next_space - current;
        if (token_len >= 255)
            token_len = 254;
        strncpy(token, current, token_len);
        token[token_len] = '\0';

        /* Check for '=' in this token */
        eq_pos = strchr(token, '=');
        if (eq_pos != NULL)
        {
            *eq_pos = '\0';
            name = token;
            value = eq_pos + 1;

            /* Remove quotes if present */
            if ((value[0] == '\'' || value[0] == '"') && 
                (value[strlen(value)-1] == '\'' || value[strlen(value)-1] == '"'))
            {
                value++;
                value[strlen(value)-1] = '\0';
            }

            add_alias(name, value);
        }
        else
        {
            /* No '=': print the alias */
            if (print_single_alias(token) == -1)
            {
                write(STDERR_FILENO, "alias: ", 7);
                write(STDERR_FILENO, token, strlen(token));
                write(STDERR_FILENO, " not found\n", 11);
                status = 1;
            }
        }

        current = next_space;
        while (*current == ' ')
            current++;
    }

    free(joined_args);
    return (status);
}
