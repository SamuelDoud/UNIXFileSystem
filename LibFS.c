#include "LibFS.h"
#include "LibDisk.h"
#include "Builder.c"

#define SUCCESS 0
// global errno value here
int osErrno;

int
FS_Boot(char *path)
{
    printf("FS_Boot %s\n", path);
    // oops, check for errors
    if (Disk_Init() == -1) {
	printf("Disk_Init() failed\n");
	osErrno = E_GENERAL;
	return -1;
    }

    // do all of the other stuff needed...

    return 0;
}

int
FS_Sync()
{
    printf("FS_Sync\n");
    return 0;
}


int
File_Create(char *file)
{
    //what is char *file? the data? The name?
    printf("FS_Create\n");
    //Build the inode for this file
    char *myInode = BuildInode();
    // place the inode in the file system
    int inodeNum = getAvailibleSector();
    //TODO error check inodeNum beyond just checking if the number is non-negative
    if (inodeNum > -1)
    {
        disk[inodeNum].data = myInode;//hopefully this is legal. Adds the created inode to the disk
    }
    //now need to add an entry to the file table? or is that handled by File_Open?
    //if file is the data, split that into chunks
    return 0;
}

int
File_Open(char *file)
{
    printf("FS_Open\n");
    return 0;
}

int
File_Read(int fd, void *buffer, int size)
{
    printf("FS_Read\n");
    return 0;
}

int
File_Write(int fd, void *buffer, int size)
{
    printf("FS_Write\n");
    return 0;
}

int
File_Seek(int fd, int offset)
{
    printf("FS_Seek\n");
    //move the index on the openfiletable element
    //fileTable[fd].index = fileTable[fd].index + offset
    return 0;
}

int
File_Close(int fd)
{
    printf("FS_Close\n");
    //delete the file from the openFileTable
    return 0;
}

int
File_Unlink(char *file)
{
    printf("FS_Unlink\n");
    return 0;
}


// directory ops
int
Dir_Create(char *path)
{
    printf("Dir_Create %s\n", path);
    return 0;
}

int
Dir_Size(char *path)
{
    printf("Dir_Size\n");
    return 0;
}

int
Dir_Read(char *path, void *buffer, int size)
{
    printf("Dir_Read\n");
    return 0;
}

int
Dir_Unlink(char *path)
{
    printf("Dir_Unlink\n");
    return 0;
}


//OUR METHODS

int Insert_Inode(char* inode)
{
    //search for an inode block that has suffcient room in the file system.
    //use the inode bitmap


    //if one is found, place it in there!
    //if not, create a new inode block

    return SUCCESS;
}
