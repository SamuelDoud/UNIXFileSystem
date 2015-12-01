#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Params.c"

bool BuildSuperBlock(char *data)
{
    //set all the chars to be null
    memset(data,NULL_TERM , SECTOR_SIZE_1);
    //create an empty and null superblock

    data[0] = (char) MAGIC_NUMBER;
    //put the magic number in the superblock and return it

    return true;
}

char *BuildDataBlock()
{
    char* dataBlock = malloc(SECTOR_SIZE_1 * sizeof(char)); //allocate SECTOR_SIZE bytes to dataBlock
    memset(dataBlock, NULL_TERM ,SECTOR_SIZE_1);//set all the chars to null
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
