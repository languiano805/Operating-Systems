// wzip.c by Leonardo Anguiano
// Submitted for CSC 139

#include <stdio.h>
#include <stdlib.h>

/*The type of compression used here is a simple form of compression called runlength encoding.
\RLE is quite simple when you encounter n ccharaters of the same type in a row, the compression will turn t
hat into the number n and a single instance of the charater
*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    FILE *fp;          // File pointer to read from files
    int count = 0;     // Count of the number of characters
    char currentChar;  // The current character being read
    char previousChar; // The previous character read
    for (int i = 1; i < argc; i++)
    {
        fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            printf("wzip: cannot open file\n");
            exit(1);
        }
        while ((currentChar = fgetc(fp)) != EOF)
        {
            if (count == 0)
            {
                previousChar = currentChar;
                count++;
            }
            else if (currentChar == previousChar)
            {
                count++;
            }
            else
            {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&previousChar, sizeof(char), 1, stdout);
                previousChar = currentChar;
                count = 1;
            }
        }
        fclose(fp);
    }
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&previousChar, sizeof(char), 1, stdout);
}
