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

/* Global variable for environment */
extern char **environ;

/* Prompt string */
#define PROMPT "$ "

/* Function prototypes */
void display_prompt(void);
char *read_command(void);
int execute_command(char *command, char *program_name);  /* <-- Added program_name parameter */
void print_error(char *program_name, char *command);
void free_command(char *command);
char *find_in_path(char *command);
int parse_command(char *command, char **argv);
int print_env(void);
int is_builtin(char **argv);
int execute_builtin(char **argv, int *status);
ssize_t _getline(char **lineptr, size_t *n, FILE *stream);
char *_strtok(char *str, const char *delim);
int is_number(char *s);
int _atoi(char *s);
int _setenv(const char *name, const char *value, int overwrite);
int _unsetenv(const char *name);
char *_getenv(const char *name);

#endif 
