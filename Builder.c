#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "LibFS.h"
#include "LibDisk.h"
#include "FileTable.h"
#include "Map.h"



//nicks comment
#define MAGIC_NUMBER 8723 //Literally a magic number that will be stored in the super block to verify data

#define SUPER_BLOCK_ID 0
#define INODE_BITMAP_ID 2
#define DIRECTORY_BITMAP_ID 1 //these previous three are also the intial indecies in the disk
#define INODE_DATA_BLOCK_ID 3 //now these are just IDs
#define DATA_BLOCK_ID 4
#define DIRECTORY_ID 5
#define NUM_DATA_BITMAP_BLOCKS 3
#define FIRST_INODE_BLOCK_INDEX 1 + 1 + NUM_DATA_BITMAP_BLOCKS
#define NUM_INODE_BLOCKS 1000 //CHANGE ME!!!!!
#define FIRST_DATA_BLOCK_INDEX NUM_INODE_BLOCKS + FIRST_INODE_BLOCK_INDEX//Data blocks begin after inodes
#define NUM_DATA_BLOCKS (NUM_SECTORS - NUM_INODE_BLOCKS - 1 - 1 - 3)//1 - 1 - 3 SUPERBLOCK - INODE BITMAP - NUM_DATA_BLOCK_BITMAP
#define AVAILIBLE 0
#define OCCUPIED 1 //Availible and occupied are merely human readable terms for the boolean true or false in the bitmaps
#define MAX_PATH_LENGTH 16


char nullChar = '\0'; // the null character in C

bool BuildSuperBlock(Sector *super)
{
    //set all the chars to be null
    memset(super->data, nullChar, SECTOR_SIZE);
    //create an empty and null superblock

    sprintf(super->data, sizeof(super->data), "%d", MAGIC_NUMBER);
    //put the magic number in the superblock and return it

    return true;
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
    int *pointers = calloc(sizeof(int), 2 + MAX_FILE_SIZE);//ERROR
    //An inode does not take up a full sector, it can be shared with other inodes
    //it needs the file size, the file type, and its pointers.
    char *inode = (char *) malloc(sizeOf(fileType) + sizeOf(size) + sizeof(pointers));
    return inode;
}
//Function takes a path and a pointer and gives it back in the form of a directory entry
char *BuildDirectoryEntry(char *name, int pointer)
{
    char ZERO = '0';
    int lengthOfDir = 20;
    char *directoryData = malloc( lengthOfDir * sizeof(char));//this is definitional. 16 chars for name (one for null term) and pointer data (this could be 2 chars using base 256!!)
    char *integerPart = malloc(sizeof(char) * (lengthOfDir - MAX_PATH_LENGTH));//there is this many characters availible for the pointer!
    sprintf(integerPart, "%d", pointer);//place pointer into a string integerPart
    //test me!! I need to verify that this inputs pointer in a way that it goes 00## instead of ##00 if there is only two digits!

    int index = 0;
    for (index = 0; index < MAX_PATH_LENGTH; index++)
    {
        directoryData[index] = name[index];
    }
    int numOfDigits = (int)(ceil(log10(pointer))+1);
    for (index = lengthOfDir - 1; index >= MAX_PATH_LENGTH; index--)
    {
        if (numOfDigits > 0)
        {
            directoryData[index] = integerPart[numOfDigits - 1];
            numOfDigits--;
        }
        else
        {
            directoryData[index] = ZERO;
        }
    }

}

char *BuildInodeBytemap()
{//What is the purpose of this method?
    bool *bitmap = malloc(MAX_NUM_FILES * sizeof(bool));//make a bitmap of the size of the max number of files
    memset(bitmap,OCCUPIED, sizeof(bitmap));//set them all to false, analogus to unoccupied
    //could use calloc..
    return ConertBitmapToBytemap(bitmap);
}

char *BuildDataBytemap()
{
    bool *bitmap = malloc(NUM_DATA_BLOCKS * sizeof(bool));//make a bitmap of the size of the max number of files
    memset(bitmap,OCCUPIED, sizeof(bitmap));//set them all to false, analogus to unoccupied
    //could use calloc..
    return ConertBitmapToBytemap(bitmap);
}
//Method alters the state of an element the bitmap to the passed bool
//Effective in deletion and creation of a file
//MUST BE VERY CAREFUL!!
//SectorNum should be the sector number that the maps understand

char *ConvertIntToString(int i)
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
int ConvertStringToInt(char *s)
{
    int i = 0;
    //take an integer and convert it ot an ASCII string
    return i;
}
//this method checks to see if the superblock passed is really a valid superblock by comparing it to a new one
bool Verify(Sector Superblock)
{
    Sector s;
    BuildSuperBlock(s);
    return s.data == Superblock.data; //this probably isn't a valid comparsion!
    //change me!!
}
//This method takes a data sector and makes it useable in the data map!
int ConvertDataSectorToMapSectorNum(int sector)
{
    if (sector < FIRST_DATA_BLOCK_INDEX)
        return sector - FIRST_DATA_BLOCK_INDEX;
    return -1;
}
int ConvertDataMapSectorToAbsolute(int dataMapSector)
{
    return dataMapSector + FIRST_DATA_BLOCK_INDEX;
}
