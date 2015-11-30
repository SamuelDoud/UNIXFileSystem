//
// Disk.h
//
// Emulates a very simple disk (no timing issues). Allows user to
// read and write to the disk just as if it was dealing with sectors
//
//

#ifndef __Disk_H__
#define __Disk_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// a few disk parameters
#define SECTOR_SIZE  512
#define NUM_SECTORS  10000
#define MAX_FILE_SIZE 30
#define ROOT_INODE 1+1+3 //the root inode is always at the first inode block
#define MAX_NUM_FILES 1000 //super block, inode blocks, inode and file bitmaps also occupy this!
#define NUM_DATA_BLOCKS NUM_SECTORS - 1 - 1 - 3 - MAX_NUM_FILES
// disk errors
typedef enum {
  E_MEM_OP,
  E_INVALID_PARAM,
  E_OPENING_FILE,
  E_WRITING_FILE,
  E_READING_FILE,
} Disk_Error_t;

typedef struct sector {
  char data[SECTOR_SIZE];
} Sector;

extern Disk_Error_t diskErrno; // used to see what happened w/ disk ops

int Disk_Init();
int Disk_Save(char* file);
int Disk_Load(char* file);
int Disk_Write(int sector, char* buffer);
int Disk_Read(int sector, char* buffer);

#endif // __Disk_H__
