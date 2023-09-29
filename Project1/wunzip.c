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