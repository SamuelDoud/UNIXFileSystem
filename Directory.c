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
    char *directoryData = calloc( sizeof(char), DIRECTORY_LENGTH + 1);
    memset(directoryData, 0, DIRECTORY_LENGTH);//$ is an illegal character
    directoryData[DIRECTORY_LENGTH] = 0;
    char *intStr = malloc(4);
    sprintf(intStr, "%d", pointer);

    int index;
    for (index = 0; index < strlen(name); index++)
    {
        directoryData[index] = name[index];
    }

    for(index = 15; index < DIRECTORY_LENGTH; index++)
    {
        directoryData[index] = intStr[index - 15];
    }
    char x = directoryData[18];
    return directoryData;
}
char *getFilename(char *dir)
{
    char *file = calloc(sizeof(char), MAX_FILENAME_LENGTH);
    file[MAX_FILENAME_LENGTH - 1] = 0;
    strncat(file, dir, MAX_FILENAME_LENGTH);
    return file;
}
int GetDirPointer(char *dir)
{
    int pointer;
    char *pointerStr = calloc(sizeof(char), DIRECTORY_LENGTH - MAX_FILENAME_LENGTH);
    strncat(pointerStr, dir + MAX_FILENAME_LENGTH, DIRECTORY_LENGTH - MAX_FILENAME_LENGTH);//verify
    pointer = atoi(pointerStr);
    return pointer;
}
int InsertDirectory(char *inodeOfParent, char *filename, Map *data, Map *inodes)
{
    int pathLength = 16;
    //first we need to find the data pointers!
    int *dataBlocksArr = malloc(MAX_NUM_SECTORS_PER_FILE * sizeof(int));
    int size;
    int numOfDataBlocks;
    char *dataBlockStr = malloc(SECTOR_SIZE_1 * sizeof(char));
    char *directoryEntryStr = malloc(pathLength * sizeof(char));
    char *emptyDirectory = malloc(pathLength * sizeof(char)); //the ideal empty directory str
    emptyDirectory = "";
    numOfDataBlocks = ReadInodeSectors(inodeOfParent, dataBlocksArr); //datablocks array now has the sector nummbers
    size = SizeOfInode(inodeOfParent);
    //go through each data block and find an open spot
    int blockIndex = 0;
    int directoryIndex;
    if (size / DIRECTORY_LENGTH == numOfDataBlocks * (SECTOR_SIZE_1 / DIRECTORY_LENGTH)) // don't try to simplify this.. algebra does not apply to int math
    { //we know that the data blocks this inode has are full... no point of trying to write to them!
        if (numOfDataBlocks < MAX_NUM_SECTORS_PER_FILE)
        {//we can allocate a new data block as the inode is not full
            int dataBlock = FindFirstOpenAndSetToClosed(data);
            if (!AddPointer(inodeOfParent, dataBlock))
            {
                return -1;
            }
            blockIndex = numOfDataBlocks;//set the index to the last spot on the array
            dataBlocksArr[numOfDataBlocks] = dataBlock;
            numOfDataBlocks++;//the array is one larger now, show that
        }
        else
        {
            return -1;
        }
    }
    for (blockIndex; blockIndex < numOfDataBlocks; blockIndex++)
    {
        //write hte data block to a string
        Disk_Read(dataBlocksArr[blockIndex], dataBlockStr);
        for (directoryIndex = 0; directoryIndex < SECTOR_SIZE_1 - DIRECTORY_LENGTH; directoryIndex += directoryIndex)
        {//write an entry to the directoryEntryStr
            memset(directoryEntryStr, NULL, sizeof(directoryEntryStr));

            int iter;
            for (iter = 0; iter < DIRECTORY_LENGTH; iter++)
            {
                directoryEntryStr[iter] = dataBlockStr[iter + directoryIndex];
            }//this routine writes the directory on dataBlockStr to directoryEntryStr
            //probably should make a function to handle this.. I'm doing it a lot
            if (strcmp(directoryEntryStr, emptyDirectory) == 0)//a empty spot
            {
                int absInode = WriteNewInodeToDisk(inodes, DIRECTORY_ID); //write a new inode to the disk
                if (absInode < 0) return -1; //the inode could not be created
                char *newDir = malloc(DIRECTORY_LENGTH * sizeof(char));//allocate space ffor a new directory
                newDir = BuildDirectoryEntry(filename, absInode); //make a directory entry
                int iterator;
                for (iterator = 0; iterator < DIRECTORY_LENGTH; iterator++)
                {//write byte-by-byte
                    dataBlockStr[directoryIndex + iterator] = newDir[iterator];
                }
                Disk_Write(dataBlocksArr[blockIndex], dataBlockStr);//since we made changes, write this
                //this writes the created directory to the parent inode
                SetSizeOfInode(inodeOfParent, DIRECTORY_LENGTH); //increase the size flag on the inode of the parent
                return absInode;
            }
        }
    }
    return-1;
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
        //printf("%s\n",EmptyArrayOfNames[index]);
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
    int index = 0;
    for (index; index < depth; index++)
    {
        char *temp = dirNames[index];
        absoluteInodePointer = Lookup(absoluteInodePointer, dirNames[index]);
        if (absoluteInodePointer == -1); //look in the current inode for the next part of the file
        {
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
    free(dirEntry);
    return -1; //not found
}
