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

    char *emptyDir = BuildDirectoryEntry("",NULL);
    for (indexOfPointers; indexOfPointers < length; indexOfPointers++)
    {
        //we are looking for a directory with the pointer of -1
        for (indexInDataBlocks = 0; indexInDataBlocks < SECTOR_SIZE_1 / DIRECTORY_LENGTH; indexInDataBlocks++)
        {
            strncat(dirEntry, dataBlock + (indexInDataBlocks * DIRECTORY_LENGTH) + (DIRECTORY_LENGTH - sizeof(int)), sizeof(int)); //check for an off by one error
            if (strcmp(dirEntry, emptyDir) == 0)//is the dirEntry equal to negative one, an illegal pointer?
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
bool RemoveDirectory(int parentInodeSectorAbsolute, char *filenameToRemove, Map *dataMap) //this does not remove the inode of the file, user handles that
{
    int maxFilenameLength = 16;
    //parentInodeSectorAbsolute Is Absolute
    //take a directory file path, lookup where it is
    //make sure that if it is a directory, its size is zero
    //get its data pointers (absolute as always with data)
    //get its inode sector (absolute ie 987 is on sector 246 and index 3)
    //use the remove function on the Maps
    //freeTable*****....


    //get the datablocks of the inode
    int parentInodeSector = parentInodeSectorAbsolute / NUM_INODES_PER_BLOCK + FIRST_INODE_BLOCK_INDEX;
    int parentInodeSectorIndex = parentInodeSector % NUM_INODES_PER_BLOCK;
    char *inodeData = GetInode(parentInodeSector, parentInodeSectorIndex);
    int *dataPointers;
    int numOfPointers = ReadInodeSectors(inodeData, dataPointers);
    int blockIndex;
    int directoryIndex;
    char *currentDirectoryEntry = malloc(sizeof(char) * maxFilenameLength);//maxFilenameLength is the maximum length of a file name
    //make maxFilenameLength a constant
    char *data = malloc(sizeof(char) * SECTOR_SIZE_1);
    for (blockIndex = 0; blockIndex < numOfPointers; blockIndex++)
    {
        Disk_Read(dataPointers[blockIndex], data);
        for (directoryIndex = 0; directoryIndex < SECTOR_SIZE_1 - DIRECTORY_LENGTH; directoryIndex+=DIRECTORY_LENGTH)
        {
            strncat(currentDirectoryEntry, data + directoryIndex, maxFilenameLength);
            if (strcmp(currentDirectoryEntry, filenameToRemove))
            {
                //found it!! remove it!!
                //remeber to change the size
                //check if the entry is the only directory on the block
                memset(data + directoryIndex, '\0', DIRECTORY_LENGTH);//wiped from data
                if (strcmp(data, BuildDataBlock()) == 0)//the data block has no meaningful data in it
                {
                    //TODO deallocate the data block from teh dataMap and the inode
                    //find the inode entry with the sector in question, it must be the last sector... we can use the size
                    //to find that sector
                    int size = SizeOfInode(inodeData);
                    int LastSectorIndex = size / SECTOR_SIZE_1;
                    snprintf(data + (LastSectorIndex + 2) * sizeof(int), sizeof(int), "%d", -1); //that should delete it
                    FreeTableOfOne(&dataMap, dataPointers[blockIndex]);//that should free the data map of the pointer in dataPointers[blockIndex]
                }
                //decrement the size of theinode
                int size = SizeOfInode(inodeData) - DIRECTORY_LENGTH;
                snprintf(inodeData,sizeof(int), "%d", size);
                InjectInode(parentInodeSector, inodeData, parentInodeSectorIndex);//InjectInode handles the Disk_Write
                Disk_Write(dataPointers[blockIndex], data); //write data to disks
            }
        }
    }

    //search for a match
    // if the match occurs on a datablock where it is the only entry, we need to deallocate the block from the map and the parent inode
    //decrement the size on the parent inode

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
//returns -1 if the path does not exist function will act as a lookup
//Super important function
int  DoesThisPathExist(char *path)
{
//return the absolute inode
    int absoluteInodePointer = 0; //the root's absolute inode, the first part on our trip
    char *dirNames[strlen(path)];
    int depth = BreakDownPathName(path, dirNames);//dirNames is being modified, do I need to pass with the & key
    int index;
    for (index = 1; index < depth; index++)
    {
        if (absoluteInodePointer = Lookup(absoluteInodePointer, dirNames[index]) == -1); //look in the current inode for the next part of the file
        {
            free(dirNames);//deallocate
            return -1;//the file does not exist in this inode
        }
    }
    //free(dirNames);//deallocate
    return absoluteInodePointer;//this is the last inode pointer, since it is positive it answers
    //the question "Does this path exist" while also providing data useful for modification of this path
}
int Lookup(int absoluteInodePointer, char *searchTerm)
{
    //look into an directory and see if it contains the filename passed, give that filenames inode number
    int blockIndex;
    int directoryIndex;
    int numOfDataBlocks;
    int inodeSector = absoluteInodePointer / NUM_INODES_PER_BLOCK + FIRST_INODE_BLOCK_INDEX;
    int inodeSectorIndex = absoluteInodePointer % NUM_INODES_PER_BLOCK;
    char *dirEntry = malloc(16 * sizeof(char)); // the name of a directory
    char *dataBlock = malloc(sizeof(char) * SECTOR_SIZE_1);//data blocks data will be stored here
    char *inode = malloc(sizeof(char) *SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);//this is where the inode will be written
    int *dataPointers;
    inode = GetInode(inodeSector, inodeSectorIndex);
    numOfDataBlocks = ReadInodeSectors(inode, dataPointers);//writer the inode's data sectors to the dataPointers array with a length of numOfDataBlcoks
    for (blockIndex = 0; blockIndex < numOfDataBlocks; blockIndex++)
    {
        Disk_Read(dataPointers[blockIndex], dataBlock);
        for(directoryIndex = 0; directoryIndex < SECTOR_SIZE_1 - DIRECTORY_LENGTH; directoryIndex += DIRECTORY_LENGTH)
        {
            strncat(dirEntry, dataBlock + directoryIndex, 16);

            if (strcmp(dirEntry, searchTerm)==0)
            {
                //match
                char *number = malloc(sizeof(char) * sizeof(int));
                strncat(number, dirEntry + 16, sizeof(int));//should write the absolute inode to the string number
                int absChildInode = atoi(number);
                free(number);
                free(inode);
                free(dataBlock);
                free(inode);
                free(dirEntry);
                free(dataPointers);
                int atoi(number);//return the integer form of that
            }
        }
    }
    free(inode);
    free(dataBlock);
    free(inode);
    free(dirEntry);
    free(dataPointers);
    return -1; //not found
}
