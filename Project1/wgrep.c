// wgrep.c by Leonardo Anguiano
// Submitted for CSC 139

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*This tool looks through a file, line by line, trying to find a user-specified search term in the line.
\If the line has the word within it, the line is printed out, otherwise it is not*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    char *searchTerm = argv[1];

    // If no files are given, read from stdin
    if (argc == 2)
    {
        char buffer[1000];
        while (fgets(buffer, 1000, stdin) != NULL)
        {
            if (strstr(buffer, searchTerm) != NULL)
            {
                printf("%s", buffer);
            }
        }
    }
    else
    {
        // Loop through each file and print its contents
        for (int i = 2; i < argc; i++)
        {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL)
            {
                printf("wgrep: cannot open file\n");
                exit(1);
            }

            char buffer[1000];
            while (fgets(buffer, 1000, fp) != NULL)
            {
                if (strstr(buffer, searchTerm) != NULL)
                {
                    printf("%s", buffer);
                }
            }
            fclose(fp);
        }
    }
}