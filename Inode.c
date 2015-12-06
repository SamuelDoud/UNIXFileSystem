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
    }
    return index;//index is the number of buffers read
}//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
bool AddPointer(char *thisInodeData, int pointerToAdd)//adds the pointerToAdd to the inode data. Returns false on Full. Updates size as well
{
    //find the first zero...
    //write the pointer to the zero spot
    int *pointers = malloc(sizeof(int) * MAX_NUM_SECTORS_PER_FILE);
    int numPointers = ReadInodeSectors(thisInodeData, pointers); //
    int index;
    if (numPointers == MAX_NUM_SECTORS_PER_FILE)
    {
        return false;
    }
    snprintf(thisInodeData + (2 * sizeof(int)) + numPointers, sizeof(int), "%d", pointerToAdd);
    Disk_Write(pointerToAdd, BuildDataBlock());
    free(pointers);
    return true; //no free space
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
        int newDataPointer;
        //there are no data blocks or it is asking for the next data block
        if (newDataPointer = FindFirstOpenAndSetToClosed(dataMap) == -1) return -1;//get a data pointer
        AddPointer(thisInodeData, newDataPointer);
        return newDataPointer;//return that pointer
    }
    //AddPointer!
    return pointers[index]; //return the pointer at that index.... maybe need to check if its valid.. ie the index is not out of size
}
int GetParentInodes(int *pointers, int originInode)
{
    //
}
char *GetInode(int sector, int index)
{
    char* inodeBuffer = malloc (sizeof(char) * SECTOR_SIZE_1);
    char* thisInode = malloc(sizeof(char) * SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    Disk_Read(sector, inodeBuffer);
    strncat(thisInode, inodeBuffer + (index * SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK), SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    return thisInode;
}
int WriteNewInodeToDisk(Map *inodeMap, int ID)
{
    int absInode = FindFirstOpenAndSetToClosed(inodeMap);//pass the inode map to allocate a inode to write
    int childInodeSector = absInode / NUM_INODES_PER_BLOCK + FIRST_INODE_BLOCK_INDEX;
    int childInodeSectorIndex = absInode % NUM_INODES_PER_BLOCK;
    InjectInode(childInodeSector, BuildInode(ID), childInodeSectorIndex); //the inode has been written
    return absInode;
}
