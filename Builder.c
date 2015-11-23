#include <stdio.h>
#include <stdbool.h>
#include <math.h>

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
    int *pointers = (int *)malloc(sizeOf(int)*(2 + MAX_FILE_SIZE));//ERROR
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeOf(fileType) + sizeOf(size) + sizeof(pointers));
    return inode;
}
char *BuildDirectory(char *name, int pointer)
{
    char *directoryData = malloc( 20 * sizeof(char));//this is definitional. 16 chars for name (one for null term) and pointer data (this could be 2 chars using base 256!!)

}
char *addBlocksToDirectory(char *dirArr, int *pointersToAddArr)
{
    //method takes an array assumed to be representing a directiory
    //finds the area to place the pointers (after any present pointers, i.e. append)
    //adds them!

    //return the directory
}
char *deleteBlocksFromDirectory(char *dirArr, int *pointersToDelete)
{
    //method takes an array assumed to be representing a directiory
    //finds the area to place the pointers (after any present pointers, i.e. append)
    //adds them!

    //Should we take the last files and move them into the holes created?
    return dirArr;
}
char BuildInodeBitmap()
{//What is the purpose of this method?
    bool *bitmap = malloc(MAX_NUM_FILES * sizeof(bool));//make a bitmap of the size of the max number of files
    memset(bitmap,false, sizeof(bitmap));//set them all to false, analogus to unoccupied
}
//Method alters the state of an element the bitmap to the passed bool
//Effective in deletion and creation of a file
char *ChangeBitmap(char *bytemapInode, int inodeNum, bool TrueOrFalse)
{
    bool *bitmap = ConvertBytemapToBitmap();
    bitmap[inodeNum] = TrueOrFalse;
    return ConvertBitmapToBytemap(bitmap);
}
bool *ConvertBytemapToBitmap(char *bytemap)
{
    //take each character, convert it to an integer. Convert that integer into binary
    //use those eight bits to represent the bitmap for that byte
    //stitch them all together to make the bitmap

    int i;
    int secondIter;
    int charAsInt;
    char *IntAsBinary;
    bool *bitmap = malloc(sizeof(bool) * (strlen(bytemap) * 8));//the bitmap will be 8 times longer than the length of the string
    for (i = 0; i < strlen(bytemap); i++)
    {
        charAsInt = (int)(bytemap[i]);
        //take the binary of the char

        //place the binary in the bitmap
        for (secondIter = 0; secondIter < 8; secondIter++)//the integer is a 8 bit number, take those eight bits and place them in the array
        {
            bitmap[i * 8 + (8 - secondIter)] = charAsInt % 2;//this will take a decimal and convert it to binary HOPEFULLY THIS IS LEGAL
            //MIGHT NEED TO USE FALSE AND TRUE
            charAsInt = charAsInt / 2;
            //see http://mathbits.com/MathBits/CompSci/Introduction/frombase10.htm for the method used
        }
    }
    return bitmap;//return the map
}
char *ConvertBitmapToBytemap(bool *bitmap)
{
    char *s;
    //take every eight bits and convert them into a character
    //stitch together all results to make the bytemap
    return s;
}
char *convertIntToString(int i)
{
    //take an integer and convert it to base 256, which is essientally a char
    int byte = 256;
    int lengthOfString = (int)(log(i) / log(byte) + 1);//how long the string will end up being
    char *s = malloc(sizeof(char) * lengthOfString);//this is how many chars will be in the array
    int counter;
    for (counter = 0; counter < lengthOfString; counter++)
    {
        s[lengthOfString - counter] = i % byte;
        i = i / byte;
        //see http://mathbits.com/MathBits/CompSci/Introduction/frombase10.htm for the method used
    }

    return s;
}
int convert StringToInt(char *s)
{
    return 0;
}

