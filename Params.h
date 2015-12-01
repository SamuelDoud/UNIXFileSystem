#ifndef PARAMS_H_INCLUDED
#define PARAMS_H_INCLUDED

#define MAGIC_NUMBER 125 //Literally just a magic number that will be stored in the super block to verify data


#define FIRST_INODE_BLOCK_INDEX 1 + 1 + NUM_DATA_BITMAP_BLOCKS
#define FIRST_DATA_BLOCK_INDEX NUM_INODE_BLOCKS + FIRST_INODE_BLOCK_INDEX//Data blocks begin after inodes
#define AVAILIBLE 0
#define OCCUPIED 1 //Availible and occupied are merely human readable terms for the boolean true or false in the bitmaps
#define MAX_PATH_LENGTH 16 //the maximum length of a path. Actually 15 but the last character is null terminator
#define DIRECTORY_LENGTH (MAX_PATH_LENGTH + 4) //the length of a path plus the size of an integer in bytes

#define MAX_NUM_OPEN_FILES 256 //how many files can be open at once. Used as the length of the fileTable

#define SUPER_BLOCKS 1 //how many superblocks there are

#define MAX_NUM_SECTORS_PER_FILE 30 //how many sectors can be in an inode

#define MAX_NUM_FILES 1000 //how many files this filesystem can hold
#define NUM_INODE_BITMAP_BLOCKS 1
#define NUM_DATA_BITMAP_BLOCKS 3

#define NUM_DATA_BLOCKS (NUM_SECTORS - NUM_INODE_BLOCKS - SUPER_BLOCKS - NUM_DATA_BITMAP_BLOCKS - NUM_INODE_BITMAP_BLOCKS)

#define NULL_TERM '\0'

#define FILE_ID 0
#define DIRECTORY_ID (FILE_ID+1)


#define NUM_INODES_PER_BLOCK 4
#define NUM_INODE_BLOCKS (MAX_NUM_FILES / NUM_INODES_PER_BLOCK)
#define INODE_BYTEMAP_LENGTH (NUM_INODE_BLOCKS / NUM_INODES_PER_BLOCK) //should be 250

#define NUM_DATA_BLOCKS_PER_CHAR 8
#define DATA_BLOCK_BYTEMAP_LENGTH (NUM_DATA_BLOCKS / NUM_DATA_BLOCKS_PER_CHAR)

#endif // PARAMS_H_INCLUDED
