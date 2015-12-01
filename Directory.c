#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Params.h"

//what does a directory look like?

//Function takes a path and a pointer and gives it back in the form of a directory entry
char *BuildDirectoryEntry(char *name, int pointer)
{
    char ZERO = '0';
    int lengthOfDir = 20;
    char *directoryData = malloc( lengthOfDir * sizeof(char));//this is definitional. 16 chars for name (one for null term) and pointer data (this could be 2 chars using base 256!!)
    char *integerPart = malloc(sizeof(char) * (lengthOfDir - MAX_PATH_LENGTH));//there is this many characters availible for the pointer!
    sprintf(integerPart, "%d", pointer);//place pointer into a string integerPart
    //test me!! I need to verify that this inputs pointer in a way that it goes 00## instead of ##00 if there is only two digits!

    int index = 0;
    for (index = 0; index < MAX_PATH_LENGTH; index++)
    {
        directoryData[index] = name[index];
    }
    int numOfDigits = (int)(ceil(log10(pointer))+1);
    for (index = lengthOfDir - 1; index >= MAX_PATH_LENGTH; index--)
    {
        if (numOfDigits > 0)
        {
            directoryData[index] = integerPart[numOfDigits - 1];
            numOfDigits--;
        }
        else
        {
            directoryData[index] = ZERO;
        }
    }
    return directoryData;
}
char *InsertDirectory(char *data, char *entry)
{
    //insert the directory in the first free spot
    //add to the size
    char *sizeChar = malloc(sizeof(int));//we need the first 4 bytes for this operaton
    int size = (int)strncat(sizeChar, data, sizeof(int));//I'm almost certain this wont work
    size = size + DIRECTORY_LENGTH;

    return data;
    //find the spot on the directtory to insert the entry
}
int GetSize(char *directory)
{
    //drectories have a size
    return 0;
}
