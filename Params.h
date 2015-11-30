#ifndef PARAMS_H_INCLUDED
#define PARAMS_H_INCLUDED

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

#define MAX_NUM_OPEN_FILES 256

#define SUPER_BLOCKS 1

#define MAX_NUM_SECTORS_PER_FILE 30

#define MAX_NUM_FILES 1000
#define NUM_INODES_PER_SECTOR 4
#define NUM_INODE_BITMAP_BLOCKS 1
#define NUM_INODE_BLOCKS MAX_NUM_FILES (MAX_NUM_FILES / NUM_INODES_PER_SECTOR)


#define NUM_DATA_BITMAP_BLOCKS 3
#define NUM_DATA_BLOCKS (NUM_SECTORS - NUM_INODE_BLOCKS - SUPER_BLOCKS - NUM_DATA_BITMAP_BLOCKS - NUM_INODE_BITMAP_BLOCKS)


#endif // PARAMS_H_INCLUDED
