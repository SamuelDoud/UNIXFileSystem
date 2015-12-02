#ifndef PARAMS_H_INCLUDED
#define PARAMS_H_INCLUDED
extern int MAGIC_NUMBER; //Literally just a magic number that will be stored in the super block to verify data
extern int SUPER_BLOCK_INDEX;
extern int NUM_SECTORS_1;
extern int SECTOR_SIZE_1;
extern int FIRST_INODE_BLOCK_INDEX;
extern int FIRST_DATA_BLOCK_INDEX;//Data blocks begin after inodes
extern int AVAILIBLE;
extern int OCCUPIED; //Availible and occupied are merely human readable terms for the boolean true or false in the bitmaps
extern int MAX_PATH_LENGTH;//the maximum length of a path. Actually 15 but the last character is null terminator
extern int DIRECTORY_LENGTH;//the length of a path plus the size of an integer in bytes
extern int MAX_NUM_OPEN_FILES;//how many files can be open at once. Used as the length of the fileTable
extern int SUPER_BLOCKS;//how many superblocks there are
extern int MAX_NUM_SECTORS_PER_FILE;//how many sectors can be in an inode
extern int MAX_NUM_FILES;//how many files this filesystem can hold
extern int NUM_INODE_BITMAP_BLOCKS;
extern int NUM_DATA_BITMAP_BLOCKS;
extern int NUM_DATA_BLOCKS;
extern int NULL_TERM;
extern int FILE_ID;
extern int DIRECTORY_ID;
extern int NUM_INODES_PER_BLOCK;
extern int NUM_INODE_BLOCKS;
extern int INODE_BYTEMAP_LENGTH; //should be 250
extern int NUM_DATA_BLOCKS_PER_CHAR;
extern int DATA_BLOCK_BYTEMAP_LENGTH;
#endif // PARAMS_H_INCLUDED
