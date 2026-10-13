#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Global variables */
extern char **environ;
extern int last_exit_status;
extern pid_t shell_pid;
extern char *prev_dir;
extern int shell_exiting;
  /* Add this for cd - */

/* Prompt string */
#define PROMPT "$ "

void free_shell_resources(void);
void shell_exit(int status);

/* Alias structure (Task 13) */
typedef struct alias_s
{
    char *name;
    char *value;
    struct alias_s *next;
} alias_t;

/* Core functions */
void display_prompt(void);
char *read_command(void);
int execute_command(char *command, char *program_name);
int execute_commands(char *commands, char *program_name);
void print_error(char *program_name, char *command);
void free_command(char *command);

/* PATH handling */
char *find_in_path(char *command);

/* Command parsing */
int parse_command(char *command, char **argv);

/* Built-ins */
int print_env(void);
int is_builtin(char **argv);
int execute_builtin(char **argv, int *status);

/* Custom implementations (Task 6 & 7) */
ssize_t _getline(char **lineptr, size_t *n, FILE *stream);
char *_strtok(char *str, const char *delim);

/* Helper functions */
int is_number(char *s);
int _atoi(char *s);

/* Environment functions (Task 9) */
char *_getenv(const char *name);
int _setenv(const char *name, const char *value, int overwrite);
int _unsetenv(const char *name);

/* cd built-in (Task 10) */
int _cd(char **argv);
void free_prev_dir(void);

/* Logical operators (Task 12) */
int get_command_status(char *command, char *program_name);
int execute_logical_ops(char *commands, char *program_name);

/* Alias (Task 13) */
int alias_builtin(char **argv);
char *get_alias(char *name);
int add_alias(char *name, char *value);
void free_aliases(void);
void print_aliases(void);
int expand_aliases(char **command);

/* Variable replacement (Task 14) */
int replace_variables(char **command, int status);

#endif  
