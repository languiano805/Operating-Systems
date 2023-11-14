// wish.c by Leonardo Anguiano
// Submitted for CSC139

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 500
#define MAX_PATHS 500
#define MAX_LINE_LENGTH 2000

// prototypes for all functions
void print_error();
void run_cmd(char **args, char **pths, int num_paths, int parallel);
void external_cmds(char **cmmds, char **pths, int num_paths);
void set_path(char **pths, int *num_paths, char *path_str);
void change_dir(char *path);

void print_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void run_cmd(char **args, char **pths, int num_paths, int parallel)
{
    // if there are more then one arg the fork

    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
        int i;
        char *path;
        char *output_file = NULL;
        // if '>' is the first char after command then throw an error
        if (args[1] != NULL && strcmp(args[1], ">") == 0)
        {
            print_error();
            exit(1);
        }
        // else if '>' is the first char in the command then throw an error
        else if (args[0] != NULL && strcmp(args[0], ">") == 0)
        {
            print_error();
            exit(1);
        }
        for (i = 0; args[i] != NULL; i++)
        {
            // if '>' is found in command than parse command
            // if there is an argument after '>' then set output_file to that argument
            if (strstr(args[i], ">") != NULL)
            {
                char *space = strchr(args[i], '>');
                if (space != NULL)
                {
                    // Pass argument starting from character after space
                    char *arg = space + 1;
                    output_file = arg;
                }
                else
                {
                    // No argument provided, change to home directory
                    output_file = NULL;
                }
                // change args to the argument before '>'
                args[i] = strtok(args[i], ">");
                // args[i] = NULL;
                break;
            }
        }
        // print the output file
        // printf("%s\n", output_file);
        for (i = 0; i < num_paths; i++)
        {
            path = malloc(strlen(pths[i]) + strlen(args[0]) + 2);
            sprintf(path, "%s/%s", pths[i], args[0]);

            if (access(path, X_OK) == 0)
            {
                if (parallel)
                {
                }
                else if (output_file != NULL)
                {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                else
                {
                    dup2(STDOUT_FILENO, STDOUT_FILENO);
                }

                execv(path, args);
            }
            free(path);
        }

        print_error();
        exit(1);
    }
    else if (pid > 0)
    {
        // parent process
        if (!parallel)
        {
            wait(NULL);
        }
    }
    else
    {
        // fork failed
        print_error();
        exit(1);
    }
}

void external_cmds(char **cmmds, char **pths, int num_paths)
{

    int i, num_args, parallel;
    char **args;
    for (i = 0; cmmds[i] != NULL; i++)
    {
        // Check for parallel cmmds
        parallel = 0;
        num_args = 0;

        if (strstr(cmmds[i], "&") != NULL)
        {
            parallel = 1;
            // Find the first space character
            char *space = strchr(cmmds[i], '&');
            if (space != NULL)
            {
                // Pass argument starting from character after space
                // char *arg = space + 1;
                cmmds[i] = strtok(cmmds[i], "&");
            }
            else
            {
                // No argument provided, change to home directory
                cmmds[i] = strtok(cmmds[i], "&");
            }
        }

        // Parse command into arguments
        args = malloc(MAX_ARGS * sizeof(char *));
        args[num_args++] = strtok(cmmds[i], " \t\n");
        while ((args[num_args] = strtok(NULL, " \t\n")) != NULL)
        {
            num_args++;
        }
        if (num_args > 0)
        {
            // Execute command
            if (num_paths > 0)
            {
                run_cmd(args, pths, num_paths, parallel);
            }
            else
            {
                // Only execute built-in cmmds
                if (strcmp(args[0], "exit") == 0)
                {
                    if (args[1] != NULL)
                    {
                        print_error();
                    }
                    else
                    {
                        exit(0);
                    }
                }
                else if (strstr(args[0], "cd") != NULL)
                {
                    // Find first space character
                    char *space = strchr(cmmds[i], ' ');
                    if (space != NULL)
                    {
                        // Pass argument starting from character after space
                        char *arg = space + 1;
                        change_dir(arg);
                    }
                    else
                    {
                        // No argument provided, change to home directory
                        change_dir(NULL);
                    }
                }
                else if (strstr(args[0], "path") != NULL)
                {
                    // Find the first space character
                    char *space = strchr(cmmds[i], ' ');
                    if (space != NULL)
                    {
                        // Pass argument starting from character after space
                        char *arg = space + 1;
                        set_path(pths, &num_paths, arg);
                    }
                    else
                    {
                        // No argument provided, change to home directory
                        set_path(pths, &num_paths, NULL);
                    }
                }
                else
                {
                    print_error();
                }
            }
        }
        free(args);
    }
}

void set_path(char **pths, int *num_paths, char *path_str)
{

    char *path;

    *num_paths = 0;

    // Parse new pths
    if (path_str != NULL)
    {
        path = strtok(path_str, "/");
        while (path != NULL)
        {
            pths[*num_paths] = malloc(strlen(path) + 1);
            strcpy(pths[*num_paths], path);
            (*num_paths)++;
            path = strtok(NULL, "/");
        }
    }

    // If pths is empty, initialize it with "/bin"
    if (*num_paths == 0)
    {
        pths[0] = malloc(strlen("/bin") + 1);
        strcpy(pths[0], "/bin");
        (*num_paths)++;
    }
}

// change directory function
void change_dir(char *path)
{
    if (path == NULL)
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
    int input_pth = 1;
    char *line = NULL;
    size_t line_size = 0;
    ssize_t line_len;
    char *cmmds[MAX_ARGS];
    int num_cmmds, i;
    char *pths[MAX_PATHS];
    int num_paths = 1;
    pths[0] = "/bin";
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
        // Parse input into cmmds
        // if input is a single '&' then ignore command and continue
        // else parse into cmmds
        if (strcmp(line, "&\n") == 0)
        {
            continue;
        }
        num_cmmds = 0;
        cmmds[num_cmmds++] = strtok(line, "&\n");
        while ((cmmds[num_cmmds] = strtok(NULL, "&\n")) != NULL)
        {
            num_cmmds++;
        }

        // Execute cmmds
        for (i = 0; i < num_cmmds; i++)
        {
            // Check for built-in cmmds
            if (strcmp(cmmds[i], "exit") == 0)
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
            else if (strstr(cmmds[i], "cd") != NULL)
            {
                // Find first space character
                char *space = strchr(cmmds[i], ' ');
                if (space != NULL)
                {
                    // Pass argument starting from character after space
                    char *arg = space + 1;
                    change_dir(arg);
                }
                else
                {
                    // No argument provided, change to home directory
                    change_dir(NULL);
                }
            }
            else if (strstr(cmmds[i], "path") != NULL)
            {
                // Find the frist space character
                char *space = strchr(cmmds[i], ' ');
                // if the given argument is null then input_pth is 0
                if (space == NULL)
                {
                    input_pth = 0;
                }
                else
                {
                    input_pth = 1;
                }

                if (space != NULL)
                {
                    // Pass argument starting from character after space
                    char *arg = space + 1;
                    set_path(pths, &num_paths, arg);
                }
                else
                {
                    // No argument provided, change to home directory
                    set_path(pths, &num_paths, NULL);
                }
            }
            else
            {
                // if the input_pth is 0 then execute the external command
                // else print error and break
                if (input_pth == 0)
                {
                    print_error();
                    break;
                }

                // Execute external command
                external_cmds(&cmmds[i], pths, num_paths);
            }
        }
    }
    return 0;
}