#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "LibDisk.h"
#include <math.h>

#define NUM_INODE_BLOCKS 1000
#define NUM_INODES_PER_BLOCK 4
#define INODE_BYTEMAP_LENGTH (NUM_INODE_BLOCKS / NUM_INODES_PER_BLOCK) //should be 250

#define NUM_DATA_BLOCKS_PER_CHAR 8
#define DATA_BLOCK_BYTEMAP_LENGTH (NUM_DATA_BLOCKS / NUM_DATA_BLOCKS_PER_CHAR)

typedef struct Map{
    int length;
    int full;
    char *bytemap;
} Map;

Map InodeMap()
{
    Map inodeMapInit;
    inodeMapInit.length = INODE_BYTEMAP_LENGTH;
    inodeMapInit.full = pow(2, NUM_INODES_PER_BLOCK) - 1;
    inodeMapInit.bytemap = calloc(inodeMapInit.length, sizeof(char));//should set all of these to zero
}
Map DataMap()
{
    Map dataMapInit;
    dataMapInit.length = DATA_BLOCK_BYTEMAP_LENGTH;
    dataMapInit.full = pow(2, NUM_DATA_BLOCKS_PER_CHAR) - 1;
    dataMapInit.bytemap = calloc(dataMapInit.length, sizeof(char));
}

#endif // MAP_H_INCLUDED
