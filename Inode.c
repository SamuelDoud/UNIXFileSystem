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
    //get the sectors... this will require some thought
    //office hours question
}//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
bool AddPointer(char *thisInodeData, int pointerToAdd);//adds the pointerToAdd to the inode data. Returns false on Full. Updates size as well
int SizeOfInode(char *thisInodeData); //return the size of the inode
