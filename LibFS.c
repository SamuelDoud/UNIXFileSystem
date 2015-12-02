#include "LibFS.h"
#include "LibDisk.h"
//#include "LibDisk.c"

#include "Builder.h"
#include "FileTable.h"
#include "Map.h"
#include "Directory.h"
#include "Params.h"

#define SUCCESS 0
#define FAILURE -1
// global errno value here
int osErrno;

//Function definitions
int FirstOpenSpotOnTheFileTable();
int GetInode(char *);
char *BreakDownPathName(char *);
bool DoesThisPathExist(char *);

static FileTableElement *fileTable;
static Map inodeMap;
static Map dataMap;

//extern Sector *disk;

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
    Disk_Write(SUPER_BLOCK_INDEX, BuildSuperBlock()); //builds the super block by passing super block array to the Disk_Write
    // do all of the other stuff needed...
    fileTable = malloc(MAX_NUM_OPEN_FILES* sizeof(FileTableElement)); // make a new file table of garbage
    //set all the fileTable elements to the initial
    int index;
    for (index = 0; index < MAX_NUM_OPEN_FILES; index++)
    {//this loop sets all the file table elements to open as defined by the initFileTableElement() function
        fileTable[index] = initFileTableElement();
    }
    //get the initial maps
    dataMap = DataMap();
    inodeMap = InodeMap();
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
    char* fileName;//TODO get the last part f
    //TODO (Evan#2#): I don't even know what this method is doing...
    //what is char *file? the data? The name?
    printf("FS_Create\n");
    //NEED some way to check if it already exists
    if (DoesThisPathExist(file))
    {
        osErrno = E_CREATE;
        return -1;
    }
    //if we get here the file does not exist!
    char *paths = BreakDownPathName(file); // this gets the parts of the path
    fileName = paths;//the fileName is going to be the last part of the path
    if (strlen(fileName) > MAX_PATH_LENGTH)
    {
        osErrno = E_FILE_TOO_BIG;//probably the wrong error code
        return -1;
    }
    //get an inode for this new file
    //TODO this is going to be a bear to debug
    int inodePointer = FIRST_INODE_BLOCK_INDEX + FindFirstOpenAndSetToClosed(&InodeMap) / inodeMap.bitsPerChar;//need the offset because inode blocks are not the zeroth seector
    int indexOfInodeInSector = (inodePointer - FIRST_INODE_BLOCK_INDEX) % inodeMap.bitsPerChar;//need to know where in the sector it is going to go
//    disk[inodePointer].data[indexOfInodeInSector * (SECTOR_SIZE / inodeMap.bitsPerChar)] = BuildInode(FILE_ID);//build a blank inode for this sector from the offset calculated
    //get the last directory inode
    //go to that data block
    char *thisFilesDirectoryEntry = BuildDirectoryEntry(fileName, inodePointer);
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
    //doesthisPathExist is not functional at this point
    if (DoesThisPathExist(file) == false)
    {
        osErrno = E_NO_SUCH_FILE;//the file does not exist
        return -1;
    }
    //if we get here, we know the file exists
    int fileDes; //variable to be the file descriptor
    if (fileDes = FirstOpenSpotOnTheFileTable() < 0)
    {
        return fileDes; // file des is already -1 and osErrno is arledy set
    }
    FileTableOpen(&fileTable[fileDes],GetInode(file), file);//opens the file table element as defined in FileTable.h
    printf("FS_Open\n");
    return fileDes; //return the file descriptor to the user
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
    char *charArr = malloc(size * sizeof(char));
    int count;
    //checking if the fd is actually real
    //check if the fd is a legal index of the file table, short ciruit if is not
    if (fd > MAX_NUM_OPEN_FILES || fd < 0 || IsGarbage(fileTable[fd]))
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    for (count = 0; count < size; count++)
    {
        charArr[count] = charAt(fd, fileTable[fd].index);
        //actually read the data, byte by byte
        fileTable[fd].index++;//move up one spot on the index
    }
    buffer = (char *)charArr;
    return count;//count is how many chars where written to the file
}

int
File_Write(int fd, void *buffer, int size)
{
    printf("FS_Write\n");
    //if the buffer is smaller than the size then an error should be thrown
    if (IsGarbage(fileTable[fd]))
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    if (size + fileTable[fd].sizeOfFile > MAX_NUM_SECTORS_PER_FILE * SECTOR_SIZE)
    {
        osErrno = E_FILE_TOO_BIG;
        return -1;
    }
    //TODO (Evan#6#): need some method to write to the next availible char
    //also beware that new sectors maybe needed so we need a method to
    //know when we need a new sector

    int currentSector;
    int indexInSector;
    int count;
    //size of file is not currently functional... not defineed to be how many bytes instead of sectors
    int offset = fileTable[fd].sizeOfFile; //offset because this the starting point of the write
    char *inode;
    Disk_Read(fileTable[fd].inodePointer, inode);
    for (count = 0 ; count < size; count++)
    {
        //TODO (Sam#8#): NOT DONE!!
    }
    fileTable[fd].sizeOfFile = fileTable[fd].sizeOfFile + count; //add how many writes where made to the file to the file table

    return count;//if all goes well then size is returned but this is how mny times there was a write made

}
//I think this is done other than the helper functions!
//size of a file is important
int
File_Seek(int fd, int offset)
{
    printf("FS_Seek\n");
    if (!IsGarbage(fileTable[fd]))//the fileTableElement is occupied
    {
        //check if the offset is less than the file size
        if (offset <= fileTable[fd].sizeOfFile)//the offset is within the size of the file
        {
            fileTable[fd].index = offset;
            return 0;
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
    if (IsGarbage(fileTable[fd]))
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    //Files can be closed by setting the inodePointer to garbage
    FileTableClose(&fileTable[fd]);//close the fileTable entry and return the success of that
    return true;
}
int
File_Unlink(char *file)
{
    //basically a deletion
    //take the file
    //find the inode
    //use the fact that FileTableElements contain the name of the file
    //search
    int index;
    for (index = 0; index < MAX_NUM_OPEN_FILES; index++)
    {
        if (strcmp(file, fileTable[index].fileName) == 0) //the two strings are the same, therefore this is the file
        {
            //code here
            //find the directory it resides in, delete that
            int *dataPointers = malloc(fileTable[index].sizeOfFile * sizeof(int));//create an array of the size sizeOfFile ints
            //dataPointers = AFunctionToGetThePointersOfAnInode(fileTable[index].inodePointer);
            FreeTableOf(dataMap, dataPointers, fileTable[index].sizeOfFile);//clear the datamap of these blocks
            FreeTableOfOne(inodeMap, fileTable[index].inodePointer); //clear the inode
            //directory still needs to be cleared out
        }
    }
    printf("FS_Unlink\n");
    osErrno = E_NO_SUCH_FILE;
    return FAILURE;
}


// directory ops
// TODO (Nick#9#): Get the directory operations up and running
int
Dir_Create(char *path)
{
    //this is really similar the other file create
    char *myPaths = BreakDownPathName(path);//myPaths now contains the paths of the directory with the last being the one to be created
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

//split the paths into parts
//for example, the path /usr/sam/etc/
//turns into {, usr, sam, etc, \0}
char *BreakDownPathName(char *file)
{
    char delimiter = '\0'; //the delimiter used throughout the project
    int depth = GetDepthOfPath(file);
    char *paths = malloc(sizeof(char) * (depth + 1));//how many elements are in the array
    char *partOfPath = calloc(sizeof(char) , strlen(file));//allocate enoungh space for nearly the entire array.
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
            indexInDepth++;//we added one to the the paths, so now we are at an index one greater
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
    char delimiter = '\\';
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
bool DoesThisPathExist(char *path)
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
    for (index = 0; index < MAX_NUM_OPEN_FILES; index++)
    {
        if (IsGarbage(fileTable[index]))//does this mean its null?
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
return NULL;
}
int GetInode(char *file)
{
    //get the inode sector for this file
    char *paths = BreakDownPathName(file);//get the paths of this file
    return -1;
}
