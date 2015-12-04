#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "Params.h"
#include "LibDisk.h"
#include "Map.h"

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
int InsertDirectory(char *inodeOfParent, char *newDirectoryEntry, Map *data, Map *inodes)
{
    int MAX_SIZE = SECTOR_SIZE_1 * MAX_NUM_SECTORS_PER_FILE;
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
    if (length * SECTOR_SIZE_1 == size)
    {
        AddPointer(inodeOfParent, FindFirstOpenAndSetToClosed(&data));
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
                char *inodeBlock = malloc(sizeof(char) * SECTOR_SIZE_1);
                Disk_Read(childInodeSector, inodeBlock);//write the inode to childInode
                //inject the inode
                InjectInode(inodeBlock,BuildInode(DIRECTORY_ID), indexInSector); //build a new full inode with the child in it
                Disk_Write(childInodeSector, inodeBlock); //write the new inode to the disk

                size += DIRECTORY_LENGTH; //add twenty to the size
                snprintf(inodeOfParent, sizeof(int), "%d", size);//write the new size to the inodeOfTheParent
                //rewrite the inode of the parent has to be handled by the calling function?

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
int BreakDownPathName(char *file, char *EmptyArrayOfNames[])
{
    int index;
    char str[strlen(file)];
    strcpy(str, file);
    const char delimiter[2] = "\\";
    char *token;
    token = strtok(str, delimiter);
    for(index = 0; token != NULL; index++)
    {
        EmptyArrayOfNames[index] = token;
        printf("%s\n",EmptyArrayOfNames[index]);
        token = strtok(NULL, delimiter);
    }
    return index;
}
int Lookup(char *file)
{
    char *paths[strlen(path)];
    int length;
    int index;
    int subIndex;
    int location;
    int numPointers;
    int *pointersArr = malloc(sizeof(int) * MAX_NUM_SECTORS_PER_FILE);
    char *dataBlockData = malloc(sizeof(char) * SECTOR_SIZE_1);
    length = BreakDownPathName(file, paths);
    //first spot is root, check there
    //root's inode is the very first inode
    int inode = 0;
    int indexInInode = 0

    //search the inode for paths[0]
    for (index = 0; index < length; index++)
    {
        char *inode = GetInode(inode, indexInInode);
        //get the data block pointers
        numPointers = ReadInodeSectors(inode, pointersArr);
        for (subIndex = 0; subIndex < numPointers; subIndex++)
        {
            if (pointersArr[subIndex] != -1)
            {
                //the data sector is not null
                Disk_Write(pointersArr[subIndex], dataBlockData);
                location = strstr(dataBlockData, paths[index]);
                if (location != NULL)
                {
                    //it was found
                    //TODO
                    //extract the pointer here
                    char *tmp = malloc(sizeof(char) * DIRECTORY_LENGTH);
                    strncat(tmp, dataBlockData + (subIndex * DIRECTORY_LENGTH) + (DIRECTORY_LENGTH - sizeof(int)), sizeof(int));//ensure this works
                    inode = atoi(tmp);
                    free(tmp);
                }
            }
        }
    }
}
bool DoesThisPathExist(char *paths)
{

}
