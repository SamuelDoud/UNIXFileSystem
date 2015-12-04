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
    if (Disk_Init() == -1)
    {
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
    {
        //this loop sets all the file table elements to open as defined by the initFileTableElement() function
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
    printf("FS_Create\n");
    int length; // length is going to represent the number of parent directories and the file name
    char *paths[strlen(file)]; //the string array which will hold the paths and the file name
    char *filename; //the name of the file
    char *absolutePath; //the path of the file...
    length = BreakDownPathName(file, paths);
    filename = paths[length - 1];
    strncat(absolutePath, file, strlen(file) - strlen(filename));//this will concatente the entire file path ecept for the file name into absolute path

    int absoluteInodeOfParent;
    //TODO DoesThisPathExist
    //get the inode of the directory that this file will reside in
    if (absoluteInodeOfParent = DoesThisPathExist(absolutePath) == -1) //absolute inode is the the inode of this files parent
    {
        //if absoluteInode is equal to -1, the parent directorry does not exist
        osErrno = E_CREATE;
        return -1;
    }
    if (strlen(filename) > MAX_PATH_LENGTH)
    {
        osErrno = E_FILE_TOO_BIG;//probably the wrong error code
        return -1;
    }
    //if we get here the file does not exist!
    int thisAbsoluteInodePointer = FindFirstOpenAndSetToClosed(&InodeMap);//find an inode to allocate for the file. Divi
    int thisInodeSector = thisAbsoluteInodePointer / NUM_INODES_PER_BLOCK + inodeMap.firstSectorIndex; //this iss the inode sector
    int thisInodeSectorIndex = thisAbsoluteInodePointer % NUM_INODES_PER_BLOCK;//need to know where in the sector it is going to go
    char *inodeSectorData;//make a character array. Does it need to be initailized?
    char *inodeEntry; //inode Entry is the individual inode
    Disk_Read(thisInodeSector, inodeSectorData); //Read the sector to InodeSector
    //inject the inode into the inode char array
    inodeEntry = BuildInode(FILE_ID); // build an inode with the file type of file
    InjectInode(thisInodeSector, inodeEntry, thisInodeSectorIndex); //write the inode entry to the inode block
    Disk_Write(thisInodeSector, inodeSectorData); //write the inode data to the disk

    //write the entry to the directory
    int parentInodeSector = absoluteInodeOfParent / NUM_INODES_PER_BLOCK + inodeMap.firstSectorIndex; //get the sector this inode is on. This maybe should be a function
    int parentInodeSectorIndex = absoluteInodeOfParent % NUM_INODES_PER_BLOCK; //get the index of the inode in the sector
    char *inodeOfDirectory = GetInode(parentInodeSector, parentInodeSectorIndex); //get the inode of the parent directoy

    char *thisFilesDirectoryEntry = BuildDirectoryEntry(filename, thisAbsoluteInodePointer);//build an entry for the directory
    int result = InsertDirectory(&inodeOfDirectory, thisFilesDirectoryEntry, &dataMap, &inodeMap); //this puts the directory into the inode
    if (result != -1)
    {
        Disk_Write(parentInodeSector, inodeOfDirectory);
        return 0;
    }
    osErrno = E_NO_SPACE; //if we get here then the directory couldn't allocate the space req'd to place a new directory
    return -1;
}
int
File_Open(char *file)
{
    //this is going to involve moving a entry to the file table
    //run around the directory pathing until the file is determined!
    //if it does not exist, return a not found error
    //if it exists, move it the open file table and return the File Descrriptor
    //doesthisPathExist is not functional at this point
    int absoluteInode;
    if (absoluteInode = DoesThisPathExist(file) == -1)
    {
        osErrno = E_NO_SUCH_FILE;//the file does not exist
        return -1;
    }
    //if we get here, we know the file exists
    int fileDes; //variable to be the file descriptor
    if ((fileDes = FirstOpenSpotOnTheFileTable()) < 0)
    {
        osErrno = E_TOO_MANY_OPEN_FILES;
        return fileDes; // file des is already -1 and osErrno is arledy set
    }
    char *filename;
    char *paths[strlen(file)];
    int length = BreakDownPathName(file, &paths);
    filename = paths[length - 1];
    FileTableOpen(&fileTable[fileDes], absoluteInode, filename);//opens the file table element as defined in FileTable.h.. shouldd make the size
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
    int count;
    //checking if the fd is actually real
    //check if the fd is a legal index of the file table, short ciruit if is not
    if (fd > MAX_NUM_OPEN_FILES || fd < 0 || IsGarbage(fileTable[fd]))
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    int *dataBlocks;
    int inodeSector = fileTable[fd].inodePointer;
    int inodeSectorIndex = fileTable[fd].indexOfInodeInSector;
    char *thisInode = GetInode(inodeSector, inodeSectorIndex);//get the particular inode
    int numbeOfPointers = ReadInodeSectors(thisInode,dataBlocks); //read the pointers of the dataBlocks to ... dataBlocks.

    //read the data starting at the offset!

    int offset = fileTable[fd].index;
    int count;
    int BlockWeAreWritingAt;
    int howManyBytesToRead;
    char *data = malloc(SECTOR_SIZE_1 * sizeof(char));
    for (count = 0; count < size;)
    {
        BlockWeAreWritingAt = (count + offset) / SECTOR_SIZE_1; //the index in dataBlocks we are interestedd in
        howManyBytesToRead = SECTOR_SIZE_1 - (count + offset) % SECTOR_SIZE_1; //how many bytes we need to read from this sector
        if (count + howManyBytesToRead > size)
        {
            howManyBytesToRead = size - count; //prevent overflow
        }
        Disk_Read(dataBlocks[BlockWeAreWritingAt], data);
        //write this to the supplied buffer
        count += strncat(buffer + (count + offset), data, howManyBytesToRead);//increment count by how many bytes were writtent to the buffer
    }
    free(data);
    free(thisInode);
    free(dataBlocks);//deallocate unneded memory
    return count;
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
        if (currentSector = GetSectorAt(inode, (count + offset) / SECTOR_SIZE_1) == 0) //this sector is empty, therefore we need a new one
        {
            //Data block at gets the data block we are writing to! If it is not allocated (as defined by the zero sector pointer) we need to allocate a new one sector to write to
            if ((currentSector = FindFirstOpenAndSetToClosed(&dataMap)) < 0) //this gets a free sector from the datamap
            {
                //if we get here the Find function could not find a data block to allocate
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
        {
            //only called on the first run through, when we expect to be writing something that is not of SECTOR_SIZE
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
            fileTable[fd].index = offset;//change the pointer to the offset
            return 0; //return the successful operation of the function
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
    return FileTableClose(&fileTable[fd]);//close the fileTable entry and return the success of that
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
//    char *myPaths = BreakDownPathName(path);//myPaths now contains the paths of the directory with the last being the one to be create
    // we need to make sure that the files before this are real.. use the first values in myPath to find this out
    char *inode = BuildInode(DIRECTORY_ID);
    int sector;
    if (sector = FindFirstOpenAndSetToClosed(&inodeMap) < 0)
    {
        //could not find a valid sector to write to
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
    //Look up the path
    //size resides in the inode of the directory
    //return SizeOfInode(inodeData);
    printf("Dir_Size\n");
    int absoluteInode;
    if (absoluteInode = DoesThisPathExist(path) < 0)
    {
        osErrno = E_NO_SUCH_FILE;
        return -1;
    }
    int inodeSector = absoluteInode / NUM_INODES_PER_BLOCK + inodeMap.firstSectorIndex;
    int inodeSectorIndex = absoluteInode % NUM_INODES_PER_BLOCK;
    return SizeOfInode(GetInode(inodeSector, inodeSectorIndex));

    return 0;
}
//what does this method do?
int
Dir_Read(char *path, void *buffer, int size)
{
    printf("Dir_Read\n");
    int DirSize = Dir_Size(path); //DirSize is the number of characterss in the directory
    if (DirSize < 0 || DirSize > size)
    {
        osErrno = E_BUFFER_TOO_SMALL;
        return -1;
    }
    int absoluteInode = DoesThisPathExist(path);
    if (absoluteInode == -1)
    {
        osErrno = E_NO_SUCH_FILE;
        return -1;
    }
    int inodeSector = absoluteInode /NUM_INODES_PER_BLOCK + inodeMap.firstSectorIndex;
    int inodeSectorIndex = absoluteInode % NUM_INODES_PER_BLOCK;
    char *thisInode = GetInode(inodeSector, inodeSectorIndex);
    int *pointers;
    int length = ReadInodeSectors(thisInode, pointers); //this function needs to be verified that it returns the actual sectors of data blocks
    //read the data
    char *dataBlock = malloc(SECTOR_SIZE_1 * sizeof(char));
    char *directoryEntries = malloc(DIRECTORY_LENGTH * sizeof(char));
    int index;
    int subIndex;
    for (index = 0; index < length; index++)
    {
        //index is each data block
        Disk_Read(pointers[index], dataBlock);
        for (subIndex = 0; subIndex + DIRECTORY_LENGTH < SECTOR_SIZE_1; subIndex+=DIRECTORY_LENGTH)
        {
            strncat(directoryEntries, dataBlock + subIndex, DIRECTORY_LENGTH);
            strncat(buffer + (SECTOR_SIZE_1 * index + subIndex), directoryEntries, DIRECTORY_LENGTH);//this should write a directory entry to the buffer
        }
    }
    free(dataBlock);
    free(pointers);
    free(thisInode);
    free(directoryEntries);//deallocate the memory assigned
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
//we may need to move this function
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
//read the data block by using the inode and an index
char *DataBlockAt(char *inode, int index)
{
    char *buffer = calloc(SECTOR_SIZE_1 , sizeof(char)); //make a string of size SECTOR_SIZE
    Disk_Read(buffer, GetSectorAt(inode, index));//read the Sector found by the GetSectorAt function from the disk to the buffer
    return buffer; //return the buffer
}
