// wish.c by Leonardo Anguiano
// Submitted for CSC139

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 512
#define MAX_PATHS 512
#define MAX_LINE_LENGTH 2048

void print_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void execute_command(char **args, char **paths, int num_paths, int parallel)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        int i;
        char *path;
        for (i = 0; i < num_paths; i++)
        {
            path = malloc(strlen(paths[i]) + strlen(args[0]) + 2);
            sprintf(path, "%s/%s", paths[i], args[0]);
            // debug

            if (access(path, X_OK) == 0)
            {
                // Found executable file
                if (parallel)
                {
                    // Redirect standard output and error to /dev/null
                    int fd = open("/dev/null", O_WRONLY);
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                execv(path, args);
                print_error();
                exit(1);
            }
            free(path);
        }
        // Could not find executable file
        print_error();
        exit(1);
    }
    else if (pid > 0)
    {
        // Parent process
        if (!parallel)
        {
            waitpid(pid, NULL, 0);
        }
    }
    else
    {
        // Fork failed
        print_error();
    }
}

void execute_commands(char **commands, char **paths, int num_paths)
{
    int i, j, num_args, parallel;
    char **args;
    for (i = 0; commands[i] != NULL; i++)
    {
        // Check for parallel commands
        parallel = 0;
        num_args = 0;
        for (j = 0; commands[i][j] != '\0'; j++)
        {
            if (commands[i][j] == '&')
            {
                parallel = 1;
                break;
            }
        }
        // Parse command into arguments
        args = malloc(MAX_ARGS * sizeof(char *));
        args[num_args++] = strtok(commands[i], " \t\n");
        while ((args[num_args] = strtok(NULL, " \t\n")) != NULL)
        {
            num_args++;
        }
        if (num_args > 0)
        {
            // Execute command
            execute_command(args, paths, num_paths, parallel);
        }
        free(args);
    }
}

void set_path(char **paths, int *num_paths, char *path_str)
{
    int i;
    char *path;
    // Clear existing paths
    for (i = 0; i < *num_paths; i++)
    {
        free(paths[i]);
    }
    *num_paths = 0;
    // Parse new paths
    path = strtok(path_str, ":");
    while (path != NULL)
    {
        paths[*num_paths] = malloc(strlen(path) + 1);
        strcpy(paths[*num_paths], path);
        (*num_paths)++;
        path = strtok(NULL, ":");
    }
}

// change directory function
void change_directory(char *path)
{
    if (path == NULL)
    {
        print_error();
    }
    else if (strtok(NULL, " \t\n") != NULL)
    {
        print_error();
    }
    else
    {
        if (chdir(path) != 0)
        {
            print_error();
        }
    }
}

int main(int argc, char **argv)
{
    char *line = NULL;
    size_t line_size = 0;
    ssize_t line_len;
    char *commands[MAX_ARGS];
    int num_commands, i;
    char *paths[MAX_PATHS];
    int num_paths = 1;
    paths[0] = "/bin";
    int interactive = 1;
    FILE *batch_file = NULL;
    // Parse command line arguments
    if (argc > 2)
    {
        print_error();
        exit(1);
    }
    else if (argc == 2)
    {
        interactive = 0;
        batch_file = fopen(argv[1], "r");
        if (batch_file == NULL)
        {
            print_error();
            exit(1);
        }
    }
    // Main loop
    while (1)
    {
        if (interactive)
        {
            printf("wish> ");
            fflush(stdout);
        }
        // Read input
        if (batch_file != NULL)
        {
            line_len = getline(&line, &line_size, batch_file);
            if (line_len == -1)
            {
                exit(0);
            }
        }
        else
        {
            line_len = getline(&line, &line_size, stdin);
            if (line_len == -1)
            {
                exit(0);
            }
        }
        // Parse input into commands
        num_commands = 0;
        commands[num_commands++] = strtok(line, "&\n");
        while ((commands[num_commands] = strtok(NULL, "&\n")) != NULL)
        {
            num_commands++;
        }
        // Execute commands
        for (i = 0; i < num_commands; i++)
        {
            // Check for built-in commands
            if (strcmp(commands[i], "exit") == 0)
            {
                if (strtok(NULL, " \t\n") != NULL)
                {
                    print_error();
                }
                else
                {
                    exit(0);
                }
            }
            else if (strcmp(commands[i], "cd") == 0)
            {
                change_directory(strtok(NULL, " \t\n"));
            }
            else if (strcmp(commands[i], "path") == 0)
            {
                set_path(paths, &num_paths, strtok(NULL, "\n"));
            }
            else
            {
                // Execute external command
                execute_commands(&commands[i], paths, num_paths);
            }
        }
    }
    return 0;
}