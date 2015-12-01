#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Params.h"
#include "Map.h"




Map InodeMap()
{
    Map inodeMapInit;
    inodeMapInit.firstSectorIndex = FIRST_INODE_BLOCK_INDEX;
    inodeMapInit.length = INODE_BYTEMAP_LENGTH;
    inodeMapInit.full = pow(2, NUM_INODES_PER_BLOCK) - 1;
    inodeMapInit.bytemap = calloc(inodeMapInit.length, sizeof(char));//should set all of these to zero
    inodeMapInit.bitsPerChar = NUM_INODES_PER_BLOCK;
    return inodeMapInit;
}
Map DataMap()
{
    Map dataMapInit;
    dataMapInit.firstSectorIndex = FIRST_DATA_BLOCK_INDEX;
    dataMapInit.length = DATA_BLOCK_BYTEMAP_LENGTH;
    dataMapInit.full = pow(2, NUM_DATA_BLOCKS_PER_CHAR) - 1;
    dataMapInit.bytemap = calloc(dataMapInit.length, sizeof(char));
    dataMapInit.bytemap = NUM_DATA_BLOCKS_PER_CHAR;
    return dataMapInit;
}
int FindFirstOpenAndSetToClosed(Map *mapArg)
{
    //go through each entry in the bytemap
    int index = 0;
    for (index = 0; index < mapArg->length; index++)
    {
        if (mapArg->bytemap[index] != mapArg->full)
        {
            //this is it!
            //convert bytemap[index] to a string
            int firstZero = IndexOfFirstZero(mapArg->bytemap[index], mapArg->length);
            //set this spot to closed

            int charRepAsInt = (int) mapArg->bytemap[index];
            charRepAsInt = charRepAsInt + pow(2,mapArg->bitsPerChar - 1 - firstZero);
            mapArg->bytemap[index] = charRepAsInt;

            return index * mapArg->length + firstZero;
        }
    }
    //osErrno = E_NO_SPACE; //if we get here all the files are loaded in memory.
    //can't use osErrno...
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

bool FreeTableOf(Map *mapArg, int *pointers, int lengthOfArray)
{
    //we have an array of pointers that are to be freeded from the table
    //really only pertains to the data bytemap
    //go through each and work it out
    int index;
    for(index = 0; index < lengthOfArray; index++)
    {
        if (!FreeTableOfOne(mapArg, pointers[index]))
        {
            return false;//a critical error has occured in the free table of function
        }//take the indexth integer in the array and set it to free
    }
    return true;
}
bool FreeTableOfOne(Map *mapArg, int pointer)
{
    int pointerwithOffsetAccounted = pointer + mapArg->firstSectorIndex;
    int indexInChar = pointerwithOffsetAccounted % mapArg->bitsPerChar; //if 33 is passed to the inode map, then it is the 8th char in the 2nd bit of the 4 bit char
    mapArg->bytemap[pointerwithOffsetAccounted] = mapArg->bytemap[pointerwithOffsetAccounted] - pow(2, mapArg->bitsPerChar - 1 - indexInChar); //TODO (Evan#1#): Verify this works
    return mapArg->bytemap[pointerwithOffsetAccounted] < mapArg->full; //if this is greater than full, then we have a critical error
}
