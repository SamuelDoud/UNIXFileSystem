#include "LibFS.h"
#include "LibDisk.h"
#include "Builder.c"
#include "FileTable.h"
#include "Map.h"

#define SUCCESS 0
// global errno value here
int osErrno;
int FirstOpenSpotOnTheFileTable();
char charAt(int fd, int index);
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
    //TODO (Evan#2#): I don't even know what this method is doing...
    //what is char *file? the data? The name?
    printf("FS_Create\n");
    //NEED some way to check if it already exists
    if (DoesPathExist(file))
    {
        osErrno = E_CREATE;
        return -1;
    }


    //Build the inode for this file
    char *myInode = BuildInode();


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

    if (DoesPathExist(file) == false)
    {
        osErrno = E_NO_SUCH_FILE;//the file does not exist
        return -1;
    }
    int fileDes;
    if (fileDes = FirstOpenSpotOnTheFileTable() == -1)
    {
        return fileDes; // file des is already -1 and osErrno is arledy set
    }
    FileTableElement thisFile;
    thisFile.inodeNum; //this needs to be set!
    fileTable[fileDes] = thisFile; //the file Des index on the file table is now equal to the file table element created
    printf("FS_Open\n");
    return fileDes;
}
//other than the charAt function, this method is complete
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
        buffer[count] = (void *) charAt(fd, fileTable[fd].index);
        //actually read the data, byte by byte
        fileTable[fd].index++;//move up one spot on the index
    }
    return count;
}

int
File_Write(int fd, void *buffer, int size)
{
    printf("FS_Write\n");

    //if the buffer is smaller than the size then an error should be thrown
    if (fileTableElement[fd] == NULL)
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    if (size + fileTable[fd].size > MAX_FILE_SIZE)
    {
        osErrno = E_FILE_TOO_BIG;
        return -1;
    }
    //TODO (Evan#6#): need some method to write to the next availible char
    //also beware that new sectors maybe needed so we need a method to
    //know when we need a new sector

    //each write NEEDS to use this
    //fileTable[fd].size++;

    return size;//if all goes well then size is returned

}
//I think this is done other than the helper functions!
//size of a file is important
int
File_Seek(int fd, int offset)
{
    printf("FS_Seek\n");
    if (fileTable[fd] != NULL)
    {
        //check if the offset is less than the file size
        if (offset <= fileTable[fd].size)
        {
            fileTable[fd].index = offset;
        }
        else
        {
            osErrno = E_SEEK_OUT_OF_BOUNDS;//the index of the file is going to be too high for the file
            return -1;
        }

    }
    else
    {
        osErrno = E_BAD_FD;
        return -1;
    }
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
// TODO (Nick#9#): Get the directory operations up and running
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
    //I think dir size is stored in the dir
    //should be a multiple of 20
    //TODO (Nick#3#): get this method going
    printf("Dir_Size\n");
    return 0;
}
//what does this method do?
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

bool Insert_Inode(FileTableElement *element)
{
    // TODO (Sam#8#): take a file table element and place its inode in the disk... some where

    //find an open inode
    //this could be a method of its own
    int inodeNum = FreeInode();//assuming that this is valid
    //TODO (Evan#8#): inject an inode at inodeNum
    element.inodeNum = inodeNum;
    UpdateInode(inodeNum);
    return SUCCESS;
}
//get a free inode location
int FreeInode()
{
    // TODO (Sam#3#): give a free inode to the calling function
    int locationOfFreeInode;
    //search the bitmap
    int index;
    for (index = 0; index < inodeBitmapLength; index++)
    {
        if (inodeMap.bitmap[index])
        {
            //probably should run some method to update the bitmap
            return index + FIRST_INODE_BLOCK_INDEX;//add the offset to account for the actual first inode
        }
    }
    osErrno = E_NO_SPACE;
    return -1;
}
//check to see if an inode is still availible
void UpdateInode(int sectorNum)
{
    int inodeMapLocation = sectorNum - FIRST_INODE_BLOCK_INDEX;//this is the location of the inode on the map
    inodeMap.bitmap[inodeMapLocation] = IsOccupied(disk[sectorNum]);
    UpdateInodeByteMapSector();//sync the bytemap with the bitmap
}
bool IsOccupied(int sectorNum)
{
    // TODO(Evan#5#): if given a sector that is an inode, determine if there is room in the inode block for one more inode entry
}
//split the paths into parts
//for example, the path /usr/sam/etc/
//turns into {, usr, sam, etc, \0}
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
int IsAChildOf(int sectorNum, char *childName)
{
       //TODO (Nick#6#): I give you a sector that is asssumed to be a directory
       //give me the inode of the child that is passed
       //if it does not exist, return -1

       //for example, /usr/ has children evan, nick, and sam
       //if I send the call IsAChildOf(/usr, evan) return  evan's inode num
       //if I send the call IsAChildOf(/usr, notReal), return -1
       return -1;
}
//how deep does this path go?
//for example, the path /usr/sam/etc/ has a depth of 4
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
    // TODO (Sam#6#): Create a method that allows us to check if a file exists ...
//Reliant on directories being created and navigation of
//directories being possible
}
int FirstOpenSpotOnTheFileTable()
{
    int index;
    for (index = 0; index < MAX_FILES_OPEN; index++)
    {
        if (fileTable[index] == null)//does this mean its null?
        {
            return index;//assuming null means availible, return this
        }
    }
    osErrno = E_TOO_MANY_OPEN_FILES;
    return -1;//if this is reached, return -1 as there are no availible spots on the file table
}
char charAt(int fd, int index)
{
    //fd is a file descriptor
    //use the inode on the fileTableElement to find a block
    //go to the index supplied
    //return that character
// TODO (Sam#6#): Reliant on inodes being implemented
}
