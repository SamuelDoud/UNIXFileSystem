#include "LibDisk.h"
#include "Builder.c"
#include <string.h>

#define MAX_FILES_OPEN 256
#define SUPER_BLOCK_INDEX 0
#define INODE_BITMAP_INDEX 1
#define DATA_BLOCK_BITMAP_INDEX 2

// the disk in memory (static makes it private to the file)
static Sector* disk;

typedef struct FileTableElement{
int inodeNum;
int fileDescriptor;
int fileAccessCount = 0;
int index = 0;
} FileTableElement;
static *FileTableElement fileTable;
// used to see what happened w/ disk ops
Disk_Error_t diskErrno;

// used for statistics
// static int lastSector = 0;
// static int seekCount = 0;

/*
 * Disk_Init
 *
 * Initializes the disk area (really just some memory for now).
 *
 * THIS FUNCTION MUST BE CALLED BEFORE ANY OTHER FUNCTION IN HERE CAN BE USED!
 *
 */
int Disk_Init()
{
    // create the disk image and fill every sector with zeroes
    disk = (Sector *) calloc(NUM_SECTORS, sizeof(Sector));
    if(disk == NULL) {
	diskErrno = E_MEM_OP;
	return -1;
    }
    //an error has not occurred, create the file table
    fileTable = (FileTableElement *)malloc(MAX_FILES_OPEN * sizeof(FileTableElement));
    disk[SUPER_BLOCK_INDEX].data = BuildSuperBlock();
    disk[INODE_BITMAP_INDEX].data = BuildInodeBitmap();
    //creates a file table of 256 null entries
    return 0;
}

/*
 * Disk_Save
 *
 * Makes sure the current disk image gets saved to memory - this
 * will overwrite an existing file with the same name so be careful
 */
int Disk_Save(char* file) {
    FILE* diskFile;

    // error check
    if (file == NULL) {
	diskErrno = E_INVALID_PARAM;
	return -1;
    }

    // open the diskFile
    if ((diskFile = fopen(file, "w")) == NULL) {
	diskErrno = E_OPENING_FILE;
	return -1;
    }

    // actually write the disk image to a file
    if ((fwrite(disk, sizeof(Sector), NUM_SECTORS, diskFile)) != NUM_SECTORS) {
	fclose(diskFile);
	diskErrno = E_WRITING_FILE;
	return -1;
    }

    // clean up and return
    fclose(diskFile);
    return 0;
}

/*
 * Disk_Load
 *
 * Loads a current disk image from disk into memory - requires that
 * the disk be created first.
 */
int Disk_Load(char* file) {
    FILE* diskFile;

    // error check
    if (file == NULL) {
	diskErrno = E_INVALID_PARAM;
	return -1;
    }

    // open the diskFile
    if ((diskFile = fopen(file, "r")) == NULL) {
	diskErrno = E_OPENING_FILE;
	return -1;
    }

    // actually read the disk image into memory
    if ((fread(disk, sizeof(Sector), NUM_SECTORS, diskFile)) != NUM_SECTORS) {
	fclose(diskFile);
	diskErrno = E_READING_FILE;
	return -1;
    }

    // clean up and return
    fclose(diskFile);
    return 0;
}

/*
 * Disk_Read
 *
 * Reads a single sector from "disk" and puts it into a buffer provided
 * by the user.
 */
int Disk_Read(int sector, char* buffer) {
    // quick error checks
    if ((sector < 0) || (sector >= NUM_SECTORS) || (buffer == NULL)) {
	diskErrno = E_INVALID_PARAM;
	return -1;
    }

    // copy the memory for the user
    if((memcpy((void*)buffer, (void*)(disk + sector), sizeof(Sector))) == NULL) {
	diskErrno = E_MEM_OP;
	return -1;
    }

    return 0;
}

/*
 * Disk_Write
 *
 * Writes a single sector from memory to "disk".
 */
int Disk_Write(int sector, char* buffer)
{
    // quick error checks
    if((sector < 0) || (sector >= NUM_SECTORS) || (buffer == NULL)) {
	diskErrno = E_INVALID_PARAM;
	return -1;
    }

    // copy the memory for the user
    if((memcpy((void*)(disk + sector), (void*)buffer, sizeof(Sector))) == NULL) {
	diskErrno = E_MEM_OP;
	return -1;
    }
    return 0;
}
//The bitmap is going to require three sectors as 512 * 8 = 4096 < NUM_SECTORS. 512 * 8 * 3 = 12288 > NUM_SECTORS
//More work to be done on this!
int getNextAvailibleSector()
{
    //search for an empty sector
    //use the bitmaps!
    char *bytemap = disk[DATA_BLOCK_BITMAP_INDEX].data;
    bool *bitmap = ConvertBytemapToBitmap(bytemap);
    int sectorNum;
    for (sectorNum = 0; sectorNum < NUM_SECTORS; sectors++)
    {
        if(bitmap[sectorNum] == false) //false is analogous to empty
        {
            return sectorNum;
        }
    }
    return -1; //code reached if no sectors are open
}
int *getAvailibleSectors(int sectorsRequested)
{
}

