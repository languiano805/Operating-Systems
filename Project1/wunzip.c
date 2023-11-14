// wunzip.c by Leonardo Anguiano
// Submitted for CSC 139

/*tool simply does the reverse of wzip tool, taking in a compressed file and
writing (to standard output again) the uncompressed result
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    FILE *fp;         // File pointer to read from files
    int count = 0;    // Count of the number of characters
    char currentChar; // which char is being read
    for (int i = 1; i < argc; i++)
    {
        fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            printf("wunzip: cannot open file\n");
            exit(1);
        }
        while (fread(&count, sizeof(int), 1, fp) == 1)
        {
            fread(&currentChar, sizeof(char), 1, fp);
            for (int j = 0; j < count; j++)
            {
                printf("%c", currentChar);
            }
        }
        fclose(fp);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
implement a command line interperter (CLI), or as it is more
commonly known as, a shell. the shell should operate in this basic way: when you type in a
command (in respone to its prompt), the shell creates a child process that executes the command you entered and then prompts for more user input when it has finished

the shell implemented will be similar to , the one run every day in unix.
*/

int main(int argc, char *argv[])
{
    // determine which mode the program is in
}

// function that determines

// Parse input into commands
num_commands = 0;
commands[num_commands++] = strtok(line, "&\n");
while ((commands[num_commands] = strtok(NULL, "&\n")) != NULL)
{
    num_commands++;
}