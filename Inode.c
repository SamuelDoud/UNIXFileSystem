#include <stdbool.h>

#include "Params.h"


bool InjectInode(char *inodeBlock, char *thisInodeData, int index)
{ //writes an inode to a block given by index
    int writeLength = (SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    int startIndexOfBlock = index * writeLength;
    int startIndexOfData;
    for (startIndexOfData = 0; startIndexOfData < writeLength; startIndexOfData++)
    {
        inodeBlock[startIndexOfData + startIndexOfBlock] = thisInodeData[startIndexOfData];
    }
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
        strncat(subStr, thisInodeData + ((index + 2) * sizeof(int)), sizeof(int));
        pointersBuffer[index]= atoi(subStr);

        if (pointersBuffer[index] == 0)
        {
            return index; //a zero was just written, any more writes are useless
        }
    }
    return index;//index is the number of buffers read
}//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
bool AddPointer(char *thisInodeData, int pointerToAdd)//adds the pointerToAdd to the inode data. Returns false on Full. Updates size as well
{
    //find the first zero...
    //write the pointer to the zero spot
    char *pointerChar = "0000"; //strings which are equal to "empty"
    char *comparator = "0000";
    int index;
    for (index = 2 * sizeof(int); index < SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK; index+=sizeof(int))
    {
        strncat(pointerChar, thisInodeData + index, sizeof(int));
        if (strcmp(pointerChar, comparator) == 0)
        {
            //free space found, place that pointer here
            snprintf(thisInodeData + index, sizeof(int), "%d", pointerToAdd);//wrote the number to the slot
            //add one to the size
            index = SizeOfInode() + 1;
            //index is now the new size of the file/directory
            //set size to index
            snprintf(thisInodeData, sizeof(int), "%d", index);
            return true;//function complleted successfully
        }
    }
    return false; //no free space
}
int SizeOfInode(char *thisInodeData) //return the size of the inode
{
    char *sizeStr = malloc(sizeof(int));
     strncat(sizeStr, thisInodeData, sizeof(int));
    int size = atoi(sizeStr);
    return size;
}
//return the sector that is the indexth element in the inode
int GetSectorAt(char *thisInodeData, int index)
{
    int *pointers;//an array that will hold the pointers
    ReadInodeSectors(thisInodeData, pointers); //read all the pointers to the array
    return pointers[index]; //return the pointer at that index.... maybe need to check if its valid.. ie the index is not out of size
}
