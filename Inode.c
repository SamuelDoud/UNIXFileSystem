#include <stdbool.h>

#include "Params.h"
#include "Map.h"


bool InjectInode(int thisInodeSector, char *thisInodeData, int index)
{ //writes an inode to a block given by index
    int writeLength = (SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    int startIndexOfBlock = index * writeLength;
    int startIndexOfData;
    char *thisInodeBlock = malloc(SECTOR_SIZE_1);
    Disk_Read(thisInodeSector, thisInodeBlock); //Read the sector to InodeSector
    int count;
    for (count = index * writeLength; count  < ((index + 1) * writeLength); count++)
    {
        thisInodeBlock[count] = thisInodeData[count - (index * writeLength)];
        char temp = thisInodeBlock[count];
    }
    Disk_Write(thisInodeSector, thisInodeBlock);
    return true; //return that this successfully completed
}//put an inode into a inode block based on the index passed
int ReadInodeSectors(char *thisInodeData, int *pointersBuffer)
{
    pointersBuffer = calloc(sizeof(int), 30);//set the array to a bunch of zeros
    //get the sectors... this will require some thought
    //office hours question
    int index;
    for (index = 0; index < SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK; index++)
    {
        char *subStr = malloc(sizeof(int));
        int start = (index + 2) * sizeof(int);
        int end = start + sizeof(int);
        int i;
        for (i = (index + 2) * sizeof(int); i < end; i++)
        {
            subStr[i - start] = thisInodeData[i];
        }

        if (pointersBuffer[index] = atoi(subStr) < FIRST_DATA_BLOCK_INDEX)//write that substring to the buffer as an integer
        {//data blocks can never  be zero
            return index; //a zero was just written, any more writes are useless
        }
        int temp = pointersBuffer[index];
    }
    return index;//index is the number of buffers read
}//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
char *AddPointer(char *thisInodeData, int pointerToAdd)//adds the pointerToAdd to the inode data. Returns false on Full.
{//also creates the new data block
    char *intStr = calloc(sizeof(char), sizeof(int));
    sprintf(intStr, "%d", pointerToAdd);
    //find the first zero...
    //write the pointer to the zero spot
    int *pointers = malloc(sizeof(int) * MAX_NUM_SECTORS_PER_FILE);
    int numPointers = ReadInodeSectors(thisInodeData, pointers); //read the current data sectors to the pointers array
    if (numPointers == MAX_NUM_SECTORS_PER_FILE)//the file is full
    {
        return false;
    }
    int start = numPointers + (sizeof(int) * 2);
    int end = start + sizeof(int);
    int index;
    for (index = 0; index < end; index++)
    {
        thisInodeData[index + start] = intStr[index];//write the integer to the last spot on the inode
    }
    //TODO ensure proper allocation... just tacking on to the end here
    Disk_Write(pointerToAdd, BuildDataBlock());//creates the new data block
    free(pointers);//deallocate the pointer array
    return thisInodeData; //no free space
}
int SizeOfInode(char *thisInodeData) //return the size of the inode
{
    char *sizeStr = malloc(sizeof(int));
    strncat(sizeStr, thisInodeData, sizeof(int));
    int size = atoi(sizeStr);
    return size;
}
int SetSizeOfInode(char *thisInodeData, int increment)
{
    if (increment == 0) return increment;
    int writeLen = 4; //how many chars we decided that size can occupy
    int currentSize = SizeOfInode(thisInodeData); // the current size of the inode
    int newSize = currentSize + increment; //this the new size of the inode
    char *intStr = calloc(sizeof(char) , writeLen);
    sprintf(intStr, "%d", newSize);//converting the new size to a string
    int index;
    for (index = 0; index < strlen(intStr); index++)
    {
        thisInodeData[index] = intStr[index];//writing the integer to the inode byte by byte
    }
    return newSize; //return the new size to the user... not really important
}
//return the sector that is the indexth element in the inode
int GetSectorAt(char *thisInodeData, int index, Map *dataMap)
{
    int *pointers;//an array that will hold the pointers
    int numOfBlocks = ReadInodeSectors(thisInodeData, pointers); //read all the pointers to the array
    if (numOfBlocks == 0 || numOfBlocks == index)
    {
        int newDataPointer= FindFirstOpenAndSetToClosed(dataMap);
        //there are no data blocks or it is asking for the next data block
        if (newDataPointer  == -1) return -1;//get a data pointer
        thisInodeData = AddPointer(thisInodeData, newDataPointer);
        return newDataPointer;//return that pointer
    }
    //AddPointer!
    return pointers[index]; //return the pointer at that index.... maybe need to check if its valid.. ie the index is not out of size
}
//get an inode from a specified location
char *GetInode(int sector, int index)
{
    int writeLen = SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK; //how many bytes an inode is
    char* inodeBuffer = malloc (sizeof(char) * SECTOR_SIZE_1); //will hold the entire inode
    char* thisInode = malloc(sizeof(char) * SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);//will hold the particular inode
    Disk_Read(sector, inodeBuffer);//read from the secto
    int start = index * writeLen;//the location on the sector we are starting at
    int end = start + writeLen;//the location we are ending at
    int index = start;
    for (index = start; index < end; index++)
    {
        thisInode[index - start] = inodeBuffer[index]; //write the full inode byte-by-byte
    }
    return thisInode;
}
//takes advantage of the conversion functions in map.h
char *GetInode(int absPointer)
{
    return GetInode(GetSector(absPointer), GetSectorIndex(absPointer));
}

//write a new, blank inode with the passed flag to the disk
int WriteNewInodeToDisk(Map *inodeMap, int ID)
{
    int absInode = FindFirstOpenAndSetToClosed(inodeMap);//pass the inode map to allocate a inode to write
    int childInodeSector = absInode / NUM_INODES_PER_BLOCK + FIRST_INODE_BLOCK_INDEX;
    int childInodeSectorIndex = absInode % NUM_INODES_PER_BLOCK;
    InjectInode(childInodeSector, BuildInode(ID), childInodeSectorIndex); //the inode has been written
    return absInode;
}
