#include "LibDisk.h"
#include "Builder.c"
#include "FileTable.h"
#include "Map.h"
#include <string.h>

#define MAX_FILES_OPEN 256
#define SUPER_BLOCK_INDEX 0
#define INODE_BITMAP_INDEX 1
#define DATA_BLOCK_BITMAP_INDEX 2
#define MAX_PATH_LENGTH 16


// the disk in memory (static makes it private to the file)
static Sector* disk;
// the inode and data bitmaps in memory
static Map inodeMap;
static Map dataMap;

//the filetable in memory
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
    //creates a file table of 256 null entries
    fileTable = calloc(sizeof(FileTableElement), MAX_FILES_OPEN);
    //built the fileTable with MAX_FILE_OPEN 0 elements of FileTableElement
    disk[SUPER_BLOCK_INDEX].data = BuildSuperBlock();//Builds the superblock which is just a magic number

    //there are three sectors of data block bitmaps... need a way to write them
    int indexOfDataBitmaps;
    int dataBitmapIndexOffset = DATA_BLOCK_BITMAP_INDEX;
    char *BytemapSplit;
    char *dataBytemap = BuildDataBytemap();//this is the data block bytemaps
    for (indexOfDataBitmaps = 0; indexOfDataBitmaps < NUM_DATA_BITMAP_BLOCKS; indexOfDataBitmaps++)
    {
            BytemapSplit = malloc(SECTOR_SIZE * sizeof(char));//allocate some memory... do I need a null terminator?
            //Take SECTOR_SIZE chars from the dataBytemap starting at (indexOfDataBitmaps * SECTOR_SIZE), store it in a block
            strncpy(BytemapSplit, dataBytemap + (indexOfDataBitmaps * SECTOR_SIZE), SECTOR_SIZE);
            //repeat until it is done writing the bytemap!
            disk[DATA_BLOCK_BITMAP_INDEX + indexOfDataBitmaps] = BytemapSplit; // maybe this works?
            free(BytemapSplit);//deallocate the allocated memory for array temp
    }
    //populate the static maps



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
int getNextAvailibleDataSector()
{
    //search for an empty sector
    //use the bitmaps!
    int sectorNum;
    for (sectorNum = 0; sectorNum < NUM_SECTORS; sectorNum++)
    {
        if(inodeMap.bitmap[sectorNum] == AVAILIBLE) //false is analogous to empty, or AVAILIBLE
        {//isn't data and inodes indecies in order???
            return ConvertDataMapSectorToAbsolute(sectorNum);
            //TODO (Sam#2#): set sectorNum to occupied? Or let user methods do that?
        }
    }
    return -1; //code reached if no sectors are open
}

//this method returns a certain number of sectors... only for data files!!!
//this is not needed for inodes because you would only need one inode at a time
//simplifies fetching the sectors for a data file
int *getAvailibleSectors(int sectorsRequested)
{
    int sectorsAlreadyCollected;

    int *sectorsFound = malloc(sectorsRequested * sizeof(int));//an integer array of teh sectors
    for (sectorsAlreadyCollected = 0; sectorsAlreadyCollected < sectorsRequested; sectorsRequested++)
    {
        int sectorFound = getNextAvailibleSector();//this integer is the next availible sector in the array
        //this won't work unless sectorFound is set to OCCUPIED
        disk[sectorFound]
        if (sectorFound < 0)
        {
            return E_GENERAL;//some out of sectors availble error

        }
        sectorsFound[sectorsAlreadyCollected] = sectorFound;//add the sector to the array of availible sectors

    }
    return sectorsFound;//return the array
}
bool setStatus(int sector, bool status)
{
    if (sector > NUM_INODE_BLOCKS)
    {
        //its a data block
        dataMap.bitmap[sector - NUM_INODE_BLOCKS] = status;
    }
    else
    {
        //its an inode
        inodeMap.bitmap[sector] = status;

    }
}
bool IsSectorEmpty(Sector s)
{
    //THIS MAY NEED TO BE USED by checking the bytemap!!!


    int sectorLength = SECTOR_SIZE; //how many characters are in a sector
    //we define a sector to be empty if ALL of its characters are null
    int index;
    for (index = 0; index < sectorLength; index++)
    {
        if (s.data[index] ! nullChar)
        {
            return false;//not null, therefore not empty
        }
    }
    return true; //all the characters have been shown to be null
}
bool WipeSector(Sector *s)
{
    s.data = memset(s.data,nullChar, SECTOR_SIZE);
    //this should wipe the passed sector
    return true;//Successful
}
void Update()
{
    //update the inode and the data bytemaps in the disk to reflect their Map structs
    updateDataBlockByteMapSector();
    UpdateInodeByteMapSector();
}
void UpdateInodeByteMapSector()
{
    //take the inode bytemap from the Map struct and store it in the disk
    disk[INODE_BITMAP_INDEX].data = inodeMap.bytemap;
}
void updateDataBlockByteMapSector()
{
    //this function is a little more complex as the DataBlockByteMap is spread over three sectors
    int indexOfDataBitmaps;
    int dataBitmapIndexOffset = DATA_BLOCK_BITMAP_INDEX;
    char *BytemapSplit;
    char *dataBytemap = dataMap.bytemap;//this is the data block bytemap
    for (indexOfDataBitmaps = 0; indexOfDataBitmaps < NUM_DATA_BITMAP_BLOCKS; indexOfDataBitmaps++)
    {
            BytemapSplit = malloc(SECTOR_SIZE * sizeof(char));//allocate some memory... do I need a null terminator?
            //Take SECTOR_SIZE chars from the dataBytemap starting at (indexOfDataBitmaps * SECTOR_SIZE), store it in a block
            strncpy(BytemapSplit, dataBytemap + (indexOfDataBitmaps * SECTOR_SIZE), SECTOR_SIZE);
            //repeat until it is done writing the bytemap!
            disk[DATA_BLOCK_BITMAP_INDEX + indexOfDataBitmaps] = BytemapSplit; // maybe this works?
            free(BytemapSplit);//deallocate the allocated memory for array temp
    }
}
