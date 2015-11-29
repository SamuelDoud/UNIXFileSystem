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
    //this is going to involve moving a entry to the file table
    //run around the directory pathing until the file is determined!
    //if it does not exist, return a not found error
    //if it exists, move it the open file table and return the File Descrriptor
    printf("FS_Open\n");
    return 0;
}

int
File_Read(int fd, void *buffer, int size)
{
    //take the file from the file table using its descriptor
    //if it does not exist, return a negative number and set osErrno to some value
    //read a set number of bytes of size to buffer
    //return the number of bytes actually in buffer
    printf("FS_Read\n");
    int count;
    for (count = 0; count < size; count++)
    {

        //actually read the data, byte by byte
        fileTable[fd].index++;//move up one spot on the index
    }
    return count + 1;
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
    //this is really similar the other file create
    char *myPaths = GetPaths(path);//myPaths now contains the paths of the directory with the last being the one to be created
    // we need to make sure that the files before this are real.. use the first values in myPath to find this out
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
char *GetPaths(char *file)
{
    char delimiter = '\0'; //the delimiter used throughout the project
    int depth = GetDepthOfPath(file);
    char *paths = malloc(sizeof(char) * (depth + 1));//how many elements are in the array
    char partOfPath = calloc(sizeof(char) , strlen(file));//allocate enoungh space for nearly the entire array.
    paths[depth] = '\0';//set the last path equal to null so we know it is useless and over
    int index;//the location we are in the passed file argument
    int indexInDepth = 0;//these two indecies serve roles as their name suggests
    int indexInCurrentPath = 0;
    for (index = 0; file[index] != '\0'; index++)//go though each in the array
    {
         if (file[index] != delimiter)//make sure we aren't going to use a backslash
         {
            partOfPath[indexInCurrentPath] = file[index];//add a character to the part of the path
            indexInCurrentPath++;//count up by one
         }
         else//encountered a delimiter, store and reset!
         {
            partOfPath[indexInCurrentPath] = '\0';//the last spot in the string is the null terminator char
            paths[indexInDepth] = partOfPath; //add part of path to the paths array
            memset(partOfPath, '\0', strlen(file));//clear the string totally.
            indexInCurrentPath = 0;//reset the count
            indexInDepth++//we added one to the the paths, so now we are at an index one greater
         }

    }
    return paths;
}
//how deep does this path go?
int GetDepthOfPath(char *file)
{
    char delimiter = '\';
    int index;
    int count = 0;
    for (index = 0; file[index] != '\0'; index++)
    {
        if (file[index] == delimiter)
        {
            count++;
        }
    }
    return count;
}
//imcomplete method!
bool DoesPathExist(char *path)
{
    return false;
    //this method takes a path and navigates to it
    //if it is reachable, return true
    //if not, return false
}
