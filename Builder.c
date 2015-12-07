#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Params.h"
#include "LibDisk.h"
#include "Map.h"

char *BuildSuperBlock()
{
    char *data = calloc(sizeof(char) , SECTOR_SIZE_1); //create an array of size sector
    //set all the chars to be null

    //create an empty and null superblock
    data[0] = (char) MAGIC_NUMBER;
    printf("%s\n", data);
    //put the magic number in the superblock and return it
    return data;
}
 //this builds a blank data block
char *BuildDataBlock()
{
    char* dataBlock = malloc(SECTOR_SIZE_1 * sizeof(char)); //allocate SECTOR_SIZE bytes to dataBlock
    memset(dataBlock, '\0' ,SECTOR_SIZE_1);//set all the chars to null
    return dataBlock;//return dataBlock
}
//builds an inode with only a flag to indicate its file type
char *BuildInode(int fileType)
{
    int size = 0;
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeof(char) * SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK); //a string of the length of an Inode
    //need to set all to -1.....
    char *negativeOne = '\0';

    inode[5] = fileType + 4;//48 is zero on the ASCII table. filetype is now on the inode as a char
    int index;
    for (index = 0; index < MAX_NUM_SECTORS_PER_FILE; index++)
    {
        inode[index + 2] = negativeOne;//this isn't going to work
    }
    return inode; //return the inode to the user
}

bool BuildRoot(Map *inodeMap)
{
    char *root = calloc(sizeof(char), SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    FindFirstOpenAndSetToClosed(inodeMap);//closes the first inode availible
    root = BuildInode(DIRECTORY_ID);
    Disk_Write(FIRST_INODE_BLOCK_INDEX, root);//can only just write to the disk like this because the disk is empty if the root is being built
    return true;
}
