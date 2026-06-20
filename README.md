# simple_shell
my simple shell project

# Simple Shell - Custom UNIX Command Interpreter

A simple UNIX command line interpreter written in C.

## Description
This shell implements basic command execution with PATH resolution, built-in commands (exit, env, setenv, unsetenv, cd), and handles command separators, logical operators, and aliases.

## Compilation
```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
