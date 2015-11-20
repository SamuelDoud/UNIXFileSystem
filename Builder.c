#include <stdio.h>

#include "LibFS.h"
#include "LibDisk.h"

#define MAGIC_NUMBER 8723
#define SUPER_BLOCK_ID 0
#define INODE_BITMAP_ID 1
#define DIRECTORY_BITMAP_ID 2
#define INODE_DATA_BLOCK_ID 3
#define DATA_BLOCK_ID 4
#define DIRECTORY_ID 5

char nullChar = '\0'; // the null character in C

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
char *BuildInode()
{
    int size = 0;
    int fileType = INODE_DATA_BLOCK_ID;
    int *pointers = (int *)malloc(sizeOf(int)*(2+ MAX_FILE_SIZE));
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeOf(fileType) + sizeOf(size) + sizeof(pointers));
    return inode;
}
char *BuildDirectory(char *root)
{
    char *directoryData = malloc( sizeof(int) * 2 + sizeof(root));
    //What does a directory look like? Probably its just like an inode
    int numberOfChildren = 0;
    int *childrenPointers;
}
