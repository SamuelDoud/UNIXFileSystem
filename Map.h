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
    return inodeMapInit;
}
Map DataMap()
{
    Map dataMapInit;
    dataMapInit.length = DATA_BLOCK_BYTEMAP_LENGTH;
    dataMapInit.full = pow(2, NUM_DATA_BLOCKS_PER_CHAR) - 1;
    dataMapInit.bytemap = calloc(dataMapInit.length, sizeof(char));
    return dataMapInit;
}
int FindFirstOpen(Map *mapArg)
{
    //go through each entry in the bytemap
    int index = 0;
    for (index = 0; index < mapArg->length; index++)
    {
        if (mapArg->bytemap[index] != full)
        {
            //this is it!
            //convert bytemap[index] to a string
            //should the position I just found be set to closed?
            int firstZero = IndexOfFirstZero(mapArg->bytemap[index], mapArg->length);
            return index * mapArg->length + firstZero;
        }
    }
    osErrno = E_NO_SPACE; //if we get here all the files are loaded in memory.
    return -1;
}
//14 (1110) should return 3
//1 (0001) shoulld return 0
int IndexOfFirstZero(int n, int b)
{
    while (n > b)
    {
        n = n % b;
        b = b / 2;
    }
    return b;
}

#endif // MAP_H_INCLUDED
