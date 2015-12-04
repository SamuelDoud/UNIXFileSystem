#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Params.h"

//what does a directory look like?
const int MAX_SIZE = SECTOR_SIZE * MAX_NUM_SECTORS;
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
int InsertDirectory(char *inodeOfParent, char *newDirectoryEntry, Map *data, Map *inodes)
{
    //take the inode of the parent
    //find its data pointers
    int size;
    char *dirEntry = malloc(sizeof(char) * sizeof(int));
    char *dataBlock = malloc(sizeof(char) * SECTOR_SIZE_1);
    int indexOfPointers = 0;
    int indexInDataBlocks;
    char *sizeStr = malloc(sizeof(int) * sizeof(char));
    strncat(sizeStr, inodeOfParent, sizeof(int) * sizeof(char));
    if (size = atoi(sizeStr) == MAX_SIZE) return -1;//the file is too large!

    int *dataPointers;
    int length = ReadInodeSectors(inodeOfParent, &dataPointers); //length is how many data blocks there are
    // TODO if the size of this inode makes it so that the next free space is on another block, we need to allocate a new block
    if (length * SECTOR_SIZE == size)
    {
        AddPointer(inodeOfParent, FindFirstOpenAndSetToClosed(&dataMap));
        length = ReadInodeSectors(inodeOfParent, &dataPointers); //length is how many data blocks there are
        indexOfPointers = length - 1;//set it to the last pointer!
    }//make sure that call is legal... ie, the sector being added is sensible
    //go through each data blocks
    //find the first open spot


    for (indexOfPointers; indexOfPointers < length; indexOfPointers++)
    {
        //we are looking for a directory with the pointer of -1
        for (indexInDataBlocks = 0; indexInDataBlocks < SECTOR_SIZE_1 / DIRECTORY_LENGTH; indexInDataBlocks++)
        {
            strncat(dirEntry, dataBlock + (indexInDataBlocks * DIRECTORY_LENGTH) + (DIRECTORY_LENGTH - sizeof(int)), sizeof(int)); //check for an off by one error
            if (atoi(dirEntry) == -1)//is the dirEntry equal to negative one, an illegal pointer?
            {
                //free spot!

                //if no open spots are found, check the size of the parent
                //if less than the max size, allocate a new block (with a new first open spot)
                //else return -1
                //find an availible inode for the child
                //return the index of that inode (after building the inode)

                int total = FindFirstOpenAndSetToClosed(&inodes); //the nth inode
                int childInodeSector = total / INODE_BYTEMAP_LENGTH; //the absolute sector ... TODO an offset may be req'd here or in the index in sector
                int indexInSector = total % INODE_BYTEMAP_LENGTH; //the index of the inode
                char *buffer = malloc(sizeof(char) * SECTOR_SIZE_1);
                Disk_Read(childInodeSector, buffer, indexInSector); //write the sector to the buffer
                //inject the inode

                InjectInode(buffer,BuildInode(DIRECTORY_ID), indexInSector); //build a new full inode with the child in it
                Disk_Write(childInodeSector, BuildInode(DIRECTORY_ID)); //write the new inode to the disk

                size += DIRECTORY_LENGTH; //add twenty to the size
                snprintf(inodeOfParent, sizeof(int), "%d", size);//write the new size to the inodeOfTheParent
                return childInodeSector; //returning the inode of the child
            }
        }

    }
    return -1; //no free spots exist... give -1 to indicate failure


}
bool RemoveDirectory(char *name, Map *inodes, Map *data)
{
    //take a directory file path, lookup where it is
    //make sure that if it is a directory, its size is zero
    //get its data pointers (absolute as always with data)
    //get its inode sector (absolute ie 987 is on sector 246 and index 3)
    //use the remove function on the Maps
    //freeTable*****....


}
int GetSize(char *directory)
{
    //drectories have a size
    return 0;
}
