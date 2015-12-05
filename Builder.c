#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Params.h"

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
char *BuildInode(int fileType)
{
    int size = 0;
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeof(char) * SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK); //a string of the length of an Inode
    //need to set all to -1.....
    char *negativeOne = "-1";
    inode[0] = (char)size;
    inode[1] = (char)fileType;
    int index;
    for (index = 0; index < MAX_NUM_SECTORS_PER_FILE; index++)
    {
        inode[index + 2] = negativeOne;
    }
    return inode; //return the inode to the user
}
//Function takes a path and a pointer and gives it back in the form of a directory entry
