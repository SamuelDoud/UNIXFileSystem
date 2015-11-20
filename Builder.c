#include <stdio.h>

#include "LibFS.h"
#include "LibDisk.h"

#define MAGIC_NUMBER 8723
char nullChar = '\0';
char *BuildSuperBlock()
{
    //CURRENTLY need to workaround c's restriction on returning arrays
    char *SuperBlock = malloc(SECTOR_SIZE * sizeof(char));
    //set all the chars to be null
    memset(SuperBlock, nullChar, SECTOR_SIZE);

    //create an empty and null superblock
    sprintf(SuperBlock, sizeof(SuperBlock), "%d", MAGIC_NUMBER);
    //put the magic number in the superblock and return it

    return SuperBlock;
}

char *BuildDataBlock()
{
    char* dataBlock = malloc(SECTOR_SIZE * sizeof(char)); //allocate SECTOR_SIZE bytes to dataBlock
    memset(dataBlock, nullChar ,SECTOR_SIZE);//set all the chars to null
    return dataBlock;//return dataBlock
}
char *BuildInode(int size, int fileType, int *pointers)
{
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeOf(fileType) + sizeOf(size) + sizeof(pointers));
    return inode;
}
