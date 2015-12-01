#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "LibFS.h"
#include "LibDisk.h"
#include "Params.h"




bool BuildSuperBlock(Sector *);



bool BuildSuperBlock(Sector *super)
{
    //set all the chars to be null
    memset(super->data,NULL_TERM , SECTOR_SIZE);
    //create an empty and null superblock

    sprintf(super->data, sizeof(super->data), "%d", MAGIC_NUMBER);
    //put the magic number in the superblock and return it

    return true;
}

char *BuildDataBlock()
{
    char* dataBlock = malloc(SECTOR_SIZE * sizeof(char)); //allocate SECTOR_SIZE bytes to dataBlock
    memset(dataBlock, NULL_TERM ,SECTOR_SIZE);//set all the chars to null
    return dataBlock;//return dataBlock
}
char *BuildInode(int fileType)
{
    int size = 0;
    int *pointers = calloc(sizeof(int), 2 + MAX_NUM_SECTORS_PER_FILE);//ERROR
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeOf(fileType) + sizeOf(size) + sizeof(pointers));
    inode[0] = (char)size;
    inode[1] = (char)fileType;
    int index;
    for (index = 0; index < MAX_NUM_SECTORS_PER_FILE; index++)
    {
        inode[index + 2] = (char) pointers[index];
    }
    return inode;
}
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

}
