//wzip.c by Leonardo Anguiano
// Submitted for CSC 139


#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct
{
    char *filename;
    char *output;
    size_t outputSize;
} ThreadData;

void *processFile(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    FILE *fp = fopen(data->filename, "r");
    if (fp == NULL)
    {
        printf("pzip: cannot open file\n");
        exit(1);
    }

    int count = 0;
    char currentChar;
    char previousChar;
    int outputIndex = 0;

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
            memcpy(data->output + outputIndex, &count, sizeof(int));
            outputIndex += sizeof(int);
            data->output[outputIndex++] = previousChar;
            previousChar = currentChar;
            count = 1;
        }
    }

    // Write the last character and its count
    memcpy(data->output + outputIndex, &count, sizeof(int));
    outputIndex += sizeof(int);
    data->output[outputIndex++] = previousChar;

    fclose(fp);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("pzip: file1 [file2 ...]\n");
        exit(1);
    }
    int numCores = sysconf(_SC_NPROCESSORS_ONLN);
    int numFiles = argc - 1;
    int numThreads = numFiles < numCores ? numFiles : numCores;

    pthread_t threads[numThreads];
    ThreadData data[numThreads];

    for (int i = 0; i < numThreads; i++)
    {
        data[i].filename = argv[i + 1];

        // Get the size of the input file
        struct stat st;
        stat(argv[i + 1], &st);
        size_t fileSize = st.st_size;

        // Allocate the output buffer based on the size of the input file
        size_t outputSize = fileSize * 5;
        data[i].output = malloc(outputSize);
        data[i].outputSize = outputSize;
        if (data[i].output == NULL)
        {
            fprintf(stderr, "Error allocating memory for output buffer\n");
            return 1;
        }
        memset(data[i].output, 0, outputSize);

        pthread_create(&threads[i], NULL, processFile, &data[i]);
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Merge the output buffers
    char lastChar = 0;
    int lastCount = 0;
    for (int i = 0; i < numThreads; i++)
    {
        int count;
        char ch;
        int j = 0;
        while (j < data[i].outputSize && (count = *(int *)(data[i].output + j)) != 0)
        {
            ch = data[i].output[j + 4];
            if (ch == lastChar)
            {
                lastCount += count;
            }
            else
            {
                if (lastChar != 0)
                {
                    fwrite(&lastCount, sizeof(int), 1, stdout);
                    fwrite(&lastChar, sizeof(char), 1, stdout);
                }
                lastChar = ch;
                lastCount = count;
            }
            j += 5;
        }
        free(data[i].output);
    }
    // Write the last character and its count
    fwrite(&lastCount, sizeof(int), 1, stdout);
    fwrite(&lastChar, sizeof(char), 1, stdout);

    return 0;
}
