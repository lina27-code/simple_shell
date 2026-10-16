#include "main.h"

/* ===== From alias.c ===== */
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

/* ===== From builtins.c ===== */
#include "main.h"

/**
 * print_env - prints the current environment
 *
 * Return: 0 on success
 */
int print_env(void)
{
    char **env = environ;

    while (*env != NULL)
    {
        write(STDOUT_FILENO, *env, strlen(*env));
        write(STDOUT_FILENO, "\n", 1);
        env++;
    }
    return (0);
}

/**
 * is_builtin - checks if a command is a built-in
 * @argv: command arguments
 *
 * Return: 1 if built-in, 0 otherwise
 */
int is_builtin(char **argv)
{
    if (argv == NULL || argv[0] == NULL)
        return (0);

    if (strcmp(argv[0], "exit") == 0)
        return (1);
    if (strcmp(argv[0], "env") == 0)
        return (1);
    if (strcmp(argv[0], "setenv") == 0)
        return (1);
    if (strcmp(argv[0], "unsetenv") == 0)
        return (1);
    if (strcmp(argv[0], "alias") == 0)
    return (1);
    if (strcmp(argv[0], "cd") == 0)
    return (1);

    return (0);
}

/**
 * execute_builtin - executes a built-in command
 * @argv: command arguments
 * @status: pointer to exit status
 *
 * Return: 1 if command was handled, 0 otherwise
 */
int execute_builtin(char **argv, int *status)
{
     int exit_status;

    if (argv == NULL || argv[0] == NULL)
        return (0);
    if (strcmp(argv[0], "alias") == 0)
{
    *status = alias_builtin(argv);
    return (1);
}

if (strcmp(argv[0], "cd") == 0)
{
    *status = _cd(argv);
    return (1);
}

    /* Handle exit with optional status */
    if (strcmp(argv[0], "exit") == 0)
    {
        if (argv[1] != NULL)
        {
            if (is_number(argv[1]))
            {
                exit_status = _atoi(argv[1]);
                last_exit_status = exit_status;
                exit(exit_status);
            }
            else
            {
                exit_status = *status;
        last_exit_status = exit_status;
        exit(exit_status);

                write(STDERR_FILENO, "./hsh: exit: Illegal number: ", 29);
                write(STDERR_FILENO, argv[1], strlen(argv[1]));
                write(STDERR_FILENO, "\n", 1);
                *status = 2;
                return (1);
            }
        }
        else
        {
            free_prev_dir(); /* Clean up */
            exit(*status);
        }
    }

    /* Handle env */
    if (strcmp(argv[0], "env") == 0)
    {
        *status = print_env();
        return (1);
    }

    /* Handle setenv */
    if (strcmp(argv[0], "setenv") == 0)
    {
        if (argv[1] == NULL || argv[2] == NULL)
        {
            write(STDERR_FILENO, "setenv: Usage: setenv VARIABLE VALUE\n", 37);
            *status = 1;
            return (1);
        }
        *status = _setenv(argv[1], argv[2], 1);
        return (1);
    }

    /* Handle unsetenv */
    if (strcmp(argv[0], "unsetenv") == 0)
    {
        if (argv[1] == NULL)
        {
            write(STDERR_FILENO, "unsetenv: Usage: unsetenv VARIABLE\n", 35);
            *status = 1;
            return (1);
        }
        *status = _unsetenv(argv[1]);
        return (1);
    }

    /* Handle cd */
    if (strcmp(argv[0], "cd") == 0)
    {
        *status = _cd(argv);
        return (1);
    }

    return (0);
}
             

/* ===== From cd.c ===== */
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

/* ===== From command_separator.c ===== */
#include "main.h"

int execute_commands(char *commands, char *program_name)
{
    char *command;
    char *commands_copy;
    int status = 0;

    if (commands == NULL || commands[0] == '\0')
        return (0);

    /* Make a copy of the commands string */
    commands_copy = strdup(commands);
    if (commands_copy == NULL)
        return (-1);

    /* Parse and execute each command separated by ; */
    command = _strtok(commands_copy, ";");
    while (command != NULL)
    {
        /* Skip leading whitespace */
        while (*command == ' ' || *command == '\t')
            command++;

        if (command[0] != '\0')
        {
            /* Execute logical operators for this command segment */
            status = execute_logical_ops(command, program_name);
        }

        command = _strtok(NULL, ";");
    }

    free(commands_copy);
    return (status);
}

/* ===== From custom_getline.c ===== */
#include "main.h"

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

/* ===== From custom_strtok.c ===== */
#include "main.h"

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

/* ===== From env_helpers.c ===== */
#include "main.h"

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

/* ===== From error.c ===== */
#include "main.h"

/**
 * print_error - prints an error message
 * @program_name: name of the program
 * @command: command that failed
 */

void print_error(char *program_name, char *command)
{
    write(STDERR_FILENO, program_name, strlen(program_name));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, command, strlen(command));
    write(STDERR_FILENO, ": No such file or directory\n", 27);
    
}

/* ===== From execute.c ===== */
#include "main.h"

/**
 * parse_command - splits a command into arguments
 * @command: the command string
 * @argv: array to store arguments
 *
 * Return: number of arguments
 */
int parse_command(char *command, char **argv)
{
    char *token;
    int i = 0;

    /* Skip leading whitespace */
    while (*command == ' ' || *command == '\t')
        command++;

    token = strtok(command, " \t");
    while (token != NULL && i < 127)
    {
        argv[i++] = token;
        token = strtok(NULL, " \t");
    }
    argv[i] = NULL;

    return (i);
}

/**
 * execute_command - executes a command
 * @command: the command to execute
 * @program_name: name of the program (for error messages)
 *
 * Return: exit status of the command
 */
int execute_command(char *command, char *program_name)
{
    char *comment_pos;
    pid_t pid;
    int status;
    int argc;
    int i;
    char *argv[128];
    char *full_path;
    
    /* Task 15: Remove comments (everything after #) */
    comment_pos = strchr(command, '#');
    if (comment_pos != NULL)
        *comment_pos = '\0';

    /* Handle empty command */
    if (command == NULL || command[0] == '\0')
        return (0);

    /* Check if command is only whitespace */
    i = 0;
    while (command[i] == ' ' || command[i] == '\t')
        i++;
    if (command[i] == '\0')
        return (0);

    /* Expand aliases before parsing */
    expand_aliases(&command);

    /* Parse command into arguments */
    argc = parse_command(command, argv);
    if (argc == 0)
        return (0);

    /* Handle built-ins */
    if (is_builtin(argv))
    {
        execute_builtin(argv, &status);
        return (status);
    }

    /* Find command in PATH */
    full_path = find_in_path(argv[0]);
    if (full_path == NULL)
    {
        
        print_error(program_name, argv[0]);
        return (127);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return (1);
    }

    if (pid == 0)
    {
        /* Child process */
        if (execve(full_path, argv, environ) == -1)
        {
            free(full_path);
            print_error(program_name, argv[0]);
            exit(127);
        }
        free(full_path);
        exit(127);
    }
    else
    {
        /* Parent process - wait for child */
        wait(&status);
        free(full_path);
    }

    return (WEXITSTATUS(status));
}

/* ===== From helpers.c ===== */
#include "main.h"

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

/* ===== From logical_ops.c ===== */
#include "main.h"

/**
 * get_command_status - executes a single command and returns its status
 * @command: the command to execute
 * @program_name: name of the program (for error messages)
 *
 * Return: exit status of the command
 */
int get_command_status(char *command, char *program_name)
{
    int status;

    /* Skip leading whitespace */
    while (*command == ' ' || *command == '\t')
        command++;

    if (command[0] == '\0')
        return (0);

    /* Execute the command */
    status = execute_command(command, program_name);
    return (status);
}

/**
 * execute_logical_ops - executes commands with && and || operators
 * @commands: string containing commands with && and || operators
 * @program_name: name of the program (for error messages)
 *
 * Return: status of the last executed command
 */
int execute_logical_ops(char *commands, char *program_name)
{
    char *commands_copy;
    char *current;
    char *next_and;
    char *next_or;
    char *next_op;
    char cmd_copy[1024];
    int cmd_len;
    int status;
    int should_execute;

    if (commands == NULL || commands[0] == '\0')
        return (0);

    commands_copy = strdup(commands);
    if (commands_copy == NULL)
        return (-1);

    /* Check if there are any operators */
    if (strstr(commands_copy, "&&") == NULL && strstr(commands_copy, "||") == NULL)
    {
        status = get_command_status(commands_copy, program_name);
        free(commands_copy);
        return (status);
    }

    current = commands_copy;
    status = 0;
    should_execute = 1;

    while (*current != '\0')
    {
        /* Skip leading whitespace */
        while (*current == ' ' || *current == '\t')
            current++;

        if (*current == '\0')
            break;

        /* Find the next operator */
        next_and = strstr(current, "&&");
        next_or = strstr(current, "||");

        if (next_and == NULL && next_or == NULL)
        {
            /* No more operators - execute the remaining command */
            if (should_execute)
                status = get_command_status(current, program_name);
            break;
        }

        /* Determine which operator comes first */
        if (next_and != NULL && (next_or == NULL || next_and < next_or))
        {
            next_op = next_and;
            cmd_len = next_op - current;

            /* Copy the command without the operator */
            if (cmd_len >= (int)sizeof(cmd_copy))
                cmd_len = sizeof(cmd_copy) - 1;
            strncpy(cmd_copy, current, cmd_len);
            cmd_copy[cmd_len] = '\0';

            if (should_execute)
                status = get_command_status(cmd_copy, program_name);

            /* For &&: only execute next if status == 0 */
            if (status == 0)
                should_execute = 1;
            else
                should_execute = 0;

            current = next_op + 2;
        }
        else if (next_or != NULL && (next_and == NULL || next_or < next_and))
        {
            next_op = next_or;
            cmd_len = next_op - current;

            /* Copy the command without the operator */
            if (cmd_len >= (int)sizeof(cmd_copy))
                cmd_len = sizeof(cmd_copy) - 1;
            strncpy(cmd_copy, current, cmd_len);
            cmd_copy[cmd_len] = '\0';

            if (should_execute)
                status = get_command_status(cmd_copy, program_name);

            /* For ||: only execute next if status != 0 */
            if (status != 0)
                should_execute = 1;
            else
                should_execute = 0;

            current = next_op + 2;
        }
        else
        {
            /* Should not happen */
            break;
        }
    }

    free(commands_copy);
    return (status);
}

/* ===== From memory.c ===== */
#include "main.h"

/**
 * free_command - frees a command string
 * @command: command to free
 */
void free_command(char *command)
{
    if (command != NULL)
        free(command);
}

/* ===== From path.c ===== */
#include "main.h"

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
    char *result = NULL;

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
            result = full_path;
            break;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return (result);
}

/* ===== From prompt.c ===== */
 #include "main.h"

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
    int i;

    nread = getline(&line, &len, stdin);
    if (nread == -1)
    {
        free(line);
        return (NULL);
    }

    /* Remove newline character */
    if (nread > 0 && line[nread - 1] == '\n')
        line[nread - 1] = '\0';
    /* Skip leading whitespace */
    i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;

    /* If line is empty or only whitespace, return empty string */
    if (line[i] == '\0')
        return (line);

    /* Remove trailing spaces (optional) */
    i = nread - 1;
    while (i >= 0 && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n'))
    {
        line[i] = '\0';
        i--;
    }


    return (line);
}

/* ===== From variables.c ===== */
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

