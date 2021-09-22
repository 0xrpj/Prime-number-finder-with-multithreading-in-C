/*
    Author: Roshan Parajuli
    Application: Prime number finder with multithreading
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define NUM_FILES 3

pthread_mutex_t mutex;
char fn[50];
char ch;
int total_lines, empty_lines, count, n = 0, totalprime = 0;
double sum = 0;
int countArr[NUM_FILES];
int *data, *arr;
static FILE *fptr[NUM_FILES];

//structure for the starting and ending position of iterations
struct iter
{
    int start;
    int end;
};

//Multithreaded function for finding prime numbers from files
void *prime(void *vars)
{
    struct iter *val = (struct iter *)vars;
    int startLimit = val->start;
    int endLimit = val->end;

    //Prime number finding algorithm
    int k = 0;
    int j, flag = 1;

    for (j = startLimit; j <= endLimit; j++)
    {
        int i, flag = 1;
        for (i = 2; i <= sqrt(arr[j]); i++)
        {
            if (arr[j] % i == 0)
            {
                flag = 0;
                break;
            }
        }
        if (arr[j] <= 1)
            flag = 0;
        else if (arr[j] == 2)
            flag = 1;
        if (flag == 1)
        {
            //Prevention of race condition
            pthread_mutex_lock(&mutex);
            fprintf(fptr[NUM_FILES], "%d\n", arr[j]);
            totalprime++;
            pthread_mutex_unlock(&mutex);
        }
        k++;
    }
    pthread_exit(0);
    fclose(fptr[NUM_FILES]);
}

void main()
{
    int i, j;

    printf(" TASK 3 - FINDING PRIME NUMBERS\n\n");

    char outputfile[30];
    printf(" Enter the name of the output file (with extension): ");
    gets(outputfile);

    //Opening all files in a loop with sprintf function and storing them in a file pointer array.
    for (i = 0; i < NUM_FILES; i++)
    {
        sprintf(fn, "PrimeData%d.txt", i + 1);
        fptr[i] = fopen(fn, "r");
    }

    fptr[NUM_FILES] = fopen(outputfile, "w");

    //Counting total number of data from the files
    int c;
    for (i = 0; i < NUM_FILES; i++)
    {
        while ((ch = fgetc(fptr[i])) != EOF)
        {
            if (ch == '\n')
            {
                if ((ch = fgetc(fptr[i])) == '\n')
                {
                    fseek(fptr[i], -1, 1);
                    empty_lines++;
                    total_lines++;
                }
                else
                {
                    total_lines++;
                }
            }
        }
        countArr[i] = total_lines - empty_lines;
        total_lines = 0;
        empty_lines = 0;

        //Rewinding pointer to the start of the pointer array
        rewind(fptr[i]);
    }

    //Calculating total number of data files on 3 files

    int temp;
    for (temp = 0; temp < NUM_FILES; temp++)
    {
        n = n + countArr[temp];
    }

    arr = malloc(n * sizeof(int));

    int index = 0;
    for (i = 0; i < NUM_FILES; i++)
    {
        for (j = 0; j < countArr[i]; j++)
        {
            fscanf(fptr[i], "%d", arr + index);
            index++;
        }

        fclose(fptr[i]);
    }

    long iterations = n;
    int threads;

    printf("\n Enter the number of threads: ");
    scanf("%d", &threads);

    //Slicing the iterations value to equally distribute between all threads.
    int sliceList[threads];
    int remainder = iterations % threads;

    for (i = 0; i < threads; i++)
    {
        sliceList[i] = iterations / threads;
    }

    for (j = 0; j < remainder; j++)
    {
        sliceList[j] += 1;
    }

    int startList[threads];
    int endList[threads];

    int l;
    for (l = 0; l < threads; l++)
    {
        if (l == 0)
            startList[l] = 0;
        else
            startList[l] = endList[l - 1] + 1;

        endList[l] = startList[l] + sliceList[l] - 1;
    }

    struct iter arr[threads];

    int k;
    for (k = 0; k < threads; k++)
    {
        arr[k].start = startList[k];
        arr[k].end = endList[k];
    }

    //Initializing mutex for its usage in the multithreaded function.
    pthread_mutex_init(&mutex, NULL);

    pthread_t threadId[threads];

    //Creating threads and joining them to let every thread finish its job.
    int m;
    for (m = 0; m < threads; m++)
    {
        pthread_create(&threadId[m], NULL, prime, &arr[m]);
    }

    int n;
    for (n = 0; n < threads; n++)
    {
        pthread_join(threadId[n], NULL);
    }

    printf("\n Total prime numbers: %d\n", totalprime);

    printf("\n\n %s is saved in the same directory", outputfile);
    free(arr);
}
