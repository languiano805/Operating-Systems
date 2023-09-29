// wcat.c by Leonardo Anguiano
// Submitted for CSC 139


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wcat: cannot open file\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }
        char buffer[1000];
        while (fgets(buffer, 1000, fp) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(fp);
    }
    return 0;
}