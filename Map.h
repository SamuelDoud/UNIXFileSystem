#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "LibDisk.h"
#define inodeBitmapLength = NUM_INODE_BLOCKS
#define inodeBytemapLength = (int)ceil(inodeBitmapLength / 8.0)
//home/samuel/Desktop/UNIXFileSystem/Builder.c|26|error: initializer element is not constant|
// TODO (Sam#1#): What am I dealing with here
#define dataBitmapLength = NUM_DATA_BLOCKS
#define dataBytemapLength = (int)ceil(dataBitmapLength / 8.0)
//map contains the data to track a bitmap and a bytemap! Should have two in the file system. One for the data and one for the inode bitmaps!
typedef struct Map{
    int lengthBitmap;//integer indicating how long the bitmap array is. Could be a short or something
    int lengthBytemap;
    bool *bitmap; //the bitmap itself
    char *bytemap;
} Map;

//use the conversion methods for the data map
bool ChangeBitmap(Map *mapArg, int sectorNum, bool TrueOrFalse)
{
    if (sectorNum < mapArg->lengthBitmap)
    {
        mapArg->bitmap[sectorNum] = TrueOrFalse;
        mapArg = ConvertBitmapToBytemap(mapArg);//make sure the bytemap is reflective!
    }
    return true;
}
//THIS METHOD IS NOT COMPLETE
bool ConvertBitmapToBytemap(Map *mapData)
{
    bool *bitmap = mapData->bitmap;
    int lengthOfBitmap = mapData->lengthBitmap;
    //Take the lenght of the bitmap and append 0's unitl the 8 divides the length of bitmap

    int lengthOfBytemap = mapData->lengthBitmap / 8; //bytemaps SHOULD be eight times shorter than bitmaps
    if (lengthOfBitmap % 8 != 0)
    {
        lengthOfBytemap++;
    }
    char *bytemap = malloc(sizeof(char) * lengthOfBytemap); //indicate how many chars will be needed for this to work
    int index;
    int bitmapIndex;
    for (index = lengthOfBytemap - 1; index >=0; index--)
    {
        for (bitmapIndex = 7; bitmapIndex >= 0; bitmapIndex--)
        {
            //math operations
            //TODO (Sam#1#): ...
            //assume this works for now
        }
    }
    free(bytemap);//deallcoate the bytemap
    //take every eight bits and convert them into a character
    //stitch together all results to make the bytemap
    return true;
}
bool UpdateBytemapOnDisk(Sector *someDisk, int startSector, int numOfSectors)
{
    //TODO (Sam#2#):
}
#endif // MAP_H_INCLUDED
