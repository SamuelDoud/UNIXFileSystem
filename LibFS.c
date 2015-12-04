#include "LibFS.h"
#include "LibDisk.h"

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
char **BreakDownPathName(char *);//this needs to be a string array! Seems like strtok()
bool DoesThisPathExist(char *);//probabbly not needed
char charAt(int fd, int index); //probably not needed
char *GetFilename(char *); //gets the file name from the BreakDownPathName function
char *DataBlockAt(char *inode, int index);

static FileTableElement *fileTable;
static Map inodeMap;
static Map dataMap;

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
    Disk_Write(FindFirstOpenAndSetToClosed(&inodeMap), BuildDirectoryEntry("/", 0));//this symbolizes the root directory
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
    if (strlen(fileName) > MAX_PATH_LENGTH)
    {
        osErrno = E_FILE_TOO_BIG;//probably the wrong error code
        return -1;
    }
    //if we get here the file does not exist!
    char *paths = BreakDownPathName(file); // this gets the parts of the path
    fileName = paths;//the fileName is going to be the last part of the path... TODO
    //the last part of paths should be a \0, so the file name should be the immediately preceding string
    //this needs to be a pointer of pointers....
    int index;
    for (index = 0; paths[index] != '\0'; index++)
    {
        //nothing here
    }
    index--; //take one off
    //now paths[index] is the file name
    fileName = paths[index];
    //get an inode for this new file
    //TODO this is going to be a bear to debug
    int inodePointer = FindFirstOpenAndSetToClosed(&InodeMap) / inodeMap.bitsPerChar;//find an inode to allocate. Dividing because this is...
    int indexOfInodeInSector = (inodePointer - FIRST_INODE_BLOCK_INDEX) % inodeMap.bitsPerChar;//need to know where in the sector it is going to go
    char *inodeSector;//make a character array. Does it need to be initailized?
    char *inodeEntry; //inode Entry is the individual inode
    Disk_Read(inodePointer, inodeSector); //Read the sector to InodeSector
    //inject the inode into the inode char array
    inodeEntry = BuildInode(FILE_ID); // build an inode with the file type of file
    //inject inodeEntry into inodeSector at indexOfInodeInSector
    Disk_Write(inodePointer, inodeSector); //write to the sector

    //disk[inodePointer].data[indexOfInodeInSector * (SECTOR_SIZE / inodeMap.bitsPerChar)] = BuildInode(FILE_ID);//build a blank inode for this sector from the offset calculated
    //get the last directory inode
    //go to that data block
    char *thisFilesDirectoryEntry = BuildDirectoryEntry(fileName, inodePointer);//build an entry for the directory


    //find the directory that this file is going into
    //all of the paths from paths[0] - paths[index - 1]
    //jump around those paths
    //if following the path leads to invalid directories throw an error

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
    if ((fileDes = FirstOpenSpotOnTheFileTable()) < 0)
    {
        return fileDes; // file des is already -1 and osErrno is arledy set
    }
    char *paths;
    //paths = BreakDownPathName(file);
    paths = strtok(file, "\\"); //this may be a better way to take the split
    char *filename = GetFilename(paths); //the file name is the last path
    FileTableOpen(&fileTable[fileDes],GetInode(file), filename);//opens the file table element as defined in FileTable.h
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
    //also beware that new sectors maybe needed so we need a method to
    //know when we need a new sector
    int currentSector;
    int writen;
    int count;
    int countBy = 1;
    //size of file is not currently functional... not defineed to be how many bytes instead of sectors
    int offset = fileTable[fd].index; //offset because this the starting point of the write
    char *inode;
    char *substring;
    Disk_Read(fileTable[fd].inodePointer, inode);//inode is now holds all the info about this inode SECTOR, still need to get the particular inode
    for (count = 0 ; count < size; count+=countBy)
    {
        //TODO (Sam#5#): This is possibly functional
<<<<<<< HEAD
        if (currentSector = DataBlockAt(inode, (count + offset) / SECTOR_SIZE) == 0) //this sector is empty, therefore we need a new one
=======
        if ((currentSector = GetSectorAt(inode, (count + offset) / SECTOR_SIZE_1) == 0) //this sector is empty, therefore we need a new one
>>>>>>> da2cd2855fe54cb2c021d1906cafdb34f604c301
        {//Data block at gets the data block we are writing to! If it is not allocated (as defined by the zero sector pointer) we need to allocate a new one sector to write to
            if ((currentSector = FindFirstOpenAndSetToClosed(&dataMap)) < 0) //this gets a free sector from the datamap
            {//if we get here the Find function could not find a data block to allocate
                osErrno = E_NO_SPACE;//error to show there is no space available
                return FAILURE;//return the error code
            }
        }
        substring = calloc(sizeof(char), SECTOR_SIZE_1);//make a string of Sector_size characters full of Zeros (not garbage as we cannot be sure how much is actually being written)
        writen = strncpy(substring, buffer + (count + offset), SECTOR_SIZE_1 - (SECTOR_SIZE_1 -(count + offset) % SECTOR_SIZE_1));//take a substring of the buffer of size SECTOR_SIZE
        //this may go out of bounds on the last edge case
        //for example, we are given 513 bytes to write from pointer 0
        //the first write is great
        //write two will write 512 bytes to the current sector but (buffer + 512) only has one char...
        //This might cause a crash
        Disk_Write(currentSector, substring);//write substring to the currentSector on the disk
        free(substring); //deallocate the memory for substring
        if (countBy == 1)
        {//only called on the first run through, when we expect to be writing something that is not of SECTOR_SIZE
            count += writen; //set the count to a multiple of SECTOR_SIZE so that it starts at the next Sector
            countBy = SECTOR_SIZE_1; //now we can increment by SECTOR_SIZE so that we always are writing from the begining of a sector
        }
    }
    fileTable[fd].sizeOfFile = fileTable[fd].sizeOfFile + count; //add how many writes where made to the file to the file table
    return count - (SECTOR_SIZE - writen);//if all goes well then size is returned but this is how mny times there was a write made... I hopes

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
            FreeTableOf(&dataMap, dataPointers, fileTable[index].sizeOfFile);//clear the datamap of these blocks
            FreeTableOfOne(&inodeMap, fileTable[index].inodePointer); //clear the inode
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
    char *myPaths = BreakDownPathName(path);//myPaths now contains the paths of the directory with the last being the one to be create
    // we need to make sure that the files before this are real.. use the first values in myPath to find this out
    char *inode = BuildInode(DIRECTORY_ID);
    int sector;
    if (sector = FindFirstOpenAndSetToClosed(&inodeMap) < 0)
    {//could not find a valid sector to write to
        osErrno = E_CREATE;
        return FAILURE;
    }
    Disk_Write(sector, inode);
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
    char *rootPath = "\\"; //this is what the root is
    if (strcmp(path, rootPath) == 0)
    {
        osErrno = E_ROOT_DIR;
        return FAILURE;
    }
    if (Dir_Size(path) != 0)
    {
        osErrno = E_DIR_NOT_EMPTY;
        return FAILURE;
    }
    //remove the directory from its parent
    //free the inode
    //free the data blocks
    printf("Dir_Unlink\n");
    return 0;
}


//OUR METHODS

//split the paths into parts
//for example, the path /usr/sam/etc/
//turns into {, usr, sam, etc, \0}
char **BreakDownPathName(char *file)
{
    char delimiter = '\0'; //the delimiter used throughout the project
    int depth = GetDepthOfPath(file);
    char **paths = malloc(sizeof(char) * MAX_PATH_LENGTH * (depth + 1));//how many elements are in the array
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
//get the inode of a file... should
int GetInode(char *file)
{
    //get the inode sector for this file

    //search the file table for "file"?
    int index;
    for (index = 0; index < MAX_NUM_OPEN_FILES; index++)
    {
        if (!IsGarbage(fileTable[index]) && strcmp(fileTable[index], file) == 0) //the file is not garbage and is equal to the name
        {
            return fileTable[index].inodePointer;
        }
    }
    osErrno = E_NO_SUCH_FILE; //set the error to..
    return -1;
}
//read the data block by using the inode and an index
char *DataBlockAt(char *inode, int index)
{
<<<<<<< HEAD
    char *buffer = malloc(SECTOR_SIZE_1 * sizeof(char)); //make a string of size SECTOR_SIZE
=======
    char *buffer = calloc(SECTOR_SIZE_1 , sizeof(char)); //make a string of size SECTOR_SIZE
>>>>>>> da2cd2855fe54cb2c021d1906cafdb34f604c301
    Disk_Read(buffer, GetSectorAt(inode, index));//read the Sector found by the GetSectorAt function from the disk to the buffer
    return buffer; //return the buffer
}

//get the last part to this array
//need to verify that this works...
char *GetFilename(char *paths)
{
    int index;
    for (index = 0; paths[index] != '\0'; index++);
    return paths[index - 1];
}
