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

static FileTableElement *fileTable;//the open file table
static Map inodeMap;
static Map dataMap;//the bitmaps for the blocks
static char *globalPath; //the path passed in FS_BOOT
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
    globalPath = malloc(sizeof(path));
    globalPath = path;
    Disk_Write(SUPER_BLOCK_INDEX, BuildSuperBlock()); //builds the super block by passing super block array to the Disk_Write
    BuildRoot(); //build root is handled entirely by the Builder source
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
    //write the bitmaps to the disk
    int sectorIndex;
    int index;
    char *sectorBuffer = calloc(sizeof(char), SECTOR_SIZE_1);
    //write the inode map
    for (sectorIndex = FIRST_INODE_BLOCK_INDEX; sectorIndex < NUM_INODE_BITMAP_BLOCKS + FIRST_INODE_BLOCK_INDEX; sectorIndex++)
    {
        memset(sectorBuffer, 0, SECTOR_SIZE_1);
        for (index = 0; index < SECTOR_SIZE_1; index++)
        {
            sectorBuffer[index] = inodeMap.bytemap[index + (sectorIndex - FIRST_INODE_BLOCK_INDEX) * SECTOR_SIZE_1];
        }
        Disk_Write(sectorIndex, sectorBuffer);
    }

    //write the data map
    for (sectorIndex = FIRST_DATA_BLOCK_INDEX; sectorIndex < NUM_DATA_BITMAP_BLOCKS + FIRST_DATA_BLOCK_INDEX; sectorIndex++)
    {
        memset(sectorBuffer, 0, SECTOR_SIZE_1);
        for (index = 0; index < SECTOR_SIZE_1; index++)
        {
            sectorBuffer[index] = dataMap.bytemap[index + (sectorIndex - FIRST_DATA_BLOCK_INDEX) * SECTOR_SIZE_1];
        }
        Disk_Write(sectorIndex, sectorBuffer);
    }

    Disk_Save(globalPath);

    return 0;
}
int
File_Create(char *file)
{
    printf("FS_Create\n");
    int length; // length is going to represent the number of parent directories and the file name
    char *paths[strlen(file)]; //the string array which will hold the paths and the file name
    char *filename = calloc(sizeof(char), MAX_FILENAME_LENGTH); //the name of the file
    length = BreakDownPathName(file, paths);
    char *absolutePath = malloc(sizeof(file) - sizeof(filename) + 2); //the path of the file...
    memcpy(filename, paths[length - 1], MAX_FILENAME_LENGTH);
    strncat(absolutePath, file, strlen(file) - strlen(filename) - 1);//this will concatente the entire file path ecept for the file name into absolute path

    int absoluteInodeOfParent = DoesThisPathExist(absolutePath);
    //get the inode of the directory that this file will reside in
    if (absoluteInodeOfParent == -1) //absolute inode is the the inode of this files parent
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
    int thisAbsoluteInodePointer = FindFirstOpenAndSetToClosed(&inodeMap);//find an inode to allocate for the file. Divi
    int thisInodeSector = thisAbsoluteInodePointer / NUM_INODES_PER_BLOCK + inodeMap.firstSectorIndex; //this iss the inode sector
    int thisInodeSectorIndex = thisAbsoluteInodePointer % NUM_INODES_PER_BLOCK;//need to know where in the sector it is going to go
    char *inodeSectorData = calloc(sizeof(char), SECTOR_SIZE_1);//make a character array. Does it need to be initailized?
    char *inodeEntry = calloc(sizeof(char), SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK); //inode Entry is the individual inode

    //inject the inode into the inode char array
    inodeEntry = BuildInode(FILE_ID); // build an inode with the file type of file
    InjectInode(thisInodeSector, inodeEntry, thisInodeSectorIndex); //write the inode entry to the inode block
    //write the entry to the directory
    int parentInodeSector = GetSector(absoluteInodeOfParent); //get the sector this inode is on. This maybe should be a function
    int parentInodeSectorIndex = GetSectorIndex(absoluteInodeOfParent); //get the index of the inode in the sector
    char *inodeOfDirectory = GetInode(parentInodeSector, parentInodeSectorIndex); //get the inode of the parent directoy
    char *thisFilesDirectoryEntry = BuildDirectoryEntry(filename, thisAbsoluteInodePointer);//build an entry for the directory
    int result = InsertDirectory(inodeOfDirectory, thisFilesDirectoryEntry, &dataMap, &inodeMap); //this puts the directory into the inode
    if (result != -1)
    {
        InjectInode(parentInodeSector, inodeOfDirectory, parentInodeSectorIndex);
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
    char *filename = calloc(sizeof(char), MAX_FILENAME_LENGTH);
    char *paths[strlen(file)];
    int length = BreakDownPathName(file, &paths);
    memcpy(filename, paths[length - 1], MAX_FILENAME_LENGTH);
    char *parentPath =  calloc(sizeof(char), strlen(file));
    strncat(parentPath, file, strlen(file) - strlen(paths[length - 1]) - 1);
    int absoluteInode = DoesThisPathExist(parentPath);

    printf("FS_Open\n");
    if (absoluteInode == -1)//check if the file passed actually exists while also getting the location of its inode
    {
        osErrno = E_NO_SUCH_FILE;//the file does not exist
        return -1;
    }
    //if we get here, we know the file exists
    int fileDes = FirstOpenSpotOnTheFileTable(); //variable to be the file descriptor
    if (fileDes < 0)
    {
        osErrno = E_TOO_MANY_OPEN_FILES;
        return fileDes; // file des is already -1 and osErrno is arledy set
    }

    FileTableOpen(&fileTable[fileDes], absoluteInode, filename);//opens the file table element as defined in FileTable.h.. shouldd make the size
    return fileDes; //return the file descriptor to the user
    free(parentPath);
    free(filename);
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
    char *bufStr = calloc(sizeof(char), size);
    int count;
    //checking if the fd is actually real
    //check if the fd is a legal index of the file table, short ciruit if is not
    if (fd > MAX_NUM_OPEN_FILES || fd < 0 || IsGarbage(fileTable[fd]))
    {
        osErrno = E_BAD_FD;
        return -1;
    }
    int *dataBlocks;
    int inodeSector = GetSector(fileTable[fd].inodePointer);
    int inodeSectorIndex = GetSectorIndex(fileTable[fd].inodePointer);//applying the logic in Map.c to get the information on this inode
    char *thisInode = GetInode(inodeSector, inodeSectorIndex);//get the particular inode
    int numberOfPointers = ReadInodeSectors(thisInode,dataBlocks); //read the pointers of the dataBlocks to ... dataBlocks.

    //read the data starting at the offset!

    int offset = fileTable[fd].index;
    int BlockWeAreReadingAt;
    int howManyBytesToRead;
    int start;
    int end;
    int index;
    char *data = malloc(SECTOR_SIZE_1 * sizeof(char));
    for (count = 0; count < size;)
    {
        BlockWeAreReadingAt = dataBlocks[(count + offset) / SECTOR_SIZE_1]; //the index in dataBlocks we are interestedd in
        howManyBytesToRead = SECTOR_SIZE_1 - (count + offset) % SECTOR_SIZE_1;
        //how many bytes we need to read from this sector, count + offset + howManyBytesToWrite should put us at the end of the sector
        if (count + howManyBytesToRead > size)
        {
            howManyBytesToRead = size - count; //prevent overflow
        }
        Disk_Read(BlockWeAreReadingAt, data);

        //write this to the supplied buffer

        index = 0;
        start = count + offset;
        end = howManyBytesToRead + start;

        for (index = 0; index + start < end; index++)
        {
            bufStr[index+start] =data[index];
        }
        count+=howManyBytesToRead;
    }
    //count = strlen(bufStr);
    printf("%d\n", strlen(bufStr));
    strncat(buffer, bufStr, count);
    free(thisInode);
    free(dataBlocks);//deallocate unneded memory
    fileTable[fd].index+=count;//increment the count on the file table
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
    char *strBuffer = calloc(sizeof(char), size);
    strBuffer = (char *) buffer; //cast as a string
    //also beware that new sectors maybe needed so we need a method to
    //know when we need a new sector
    int currentSector;
    int written = 0;
    int count;
    int countBy = 1;
    //size of file is not currently functional... not defineed to be how many bytes instead of sectors
    int offset = fileTable[fd].index; //offset because this the starting point of the write
    char *inodeBlock = calloc(sizeof(char), SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK); //where the inode will be
    char *substring = calloc(sizeof(char), SECTOR_SIZE_1);
    int sectorOfInode = GetSector(fileTable[fd].inodePointer);
    int indexOfInode = GetSectorIndex(fileTable[fd].inodePointer);
    inodeBlock = GetInode(sectorOfInode, indexOfInode); //write the inode at the given location to the inodeBlock string
    int writeLen;
    int index;
    int start;
    int end;
    for (count = 0 ; count < size; count+=countBy)
    {
        writeLen = SECTOR_SIZE_1;
        if ((count + offset) % SECTOR_SIZE_1 != 0)
        {
            writeLen = SECTOR_SIZE_1 - (count + offset);
        }
        if ((count + offset) + SECTOR_SIZE_1 > size)
        {
            writeLen = size - count + offset;
        }
        currentSector = GetSectorAt(inodeBlock, (count + offset) / SECTOR_SIZE_1, &dataMap);
        if (currentSector < FIRST_DATA_BLOCK_INDEX) //couldn't allocate a valid sector
        {
            osErrno = E_FILE_TOO_BIG;
            return -1;
        }
        memset(substring, 0, SECTOR_SIZE_1);//reset the substring
        start = count;
        end = start + writeLen;
        for(index = 0; index < end - start; index++)
        {
            substring[index] = strBuffer[start + index];
        }
        written+=(writeLen);//increment  written
        Disk_Write(currentSector, substring);//write substring to the currentSector on the disk
        //free(substring); //deallocate the memory for substring
        if (countBy == 1)
        {
            //only called on the first run through, when we expect to be writing something that is not of SECTOR_SIZE
            countBy = SECTOR_SIZE_1; //now we can increment by SECTOR_SIZE so that we always are writing from the begining of a sector
        }
    }
    fileTable[fd].sizeOfFile = fileTable[fd].sizeOfFile + written; //add how many writes where made to the file to the file table
    //write the new size to the inode as well
    SetSizeOfInode(inodeBlock, written);
    char temp = inodeBlock[10];
    InjectInode(sectorOfInode, inodeBlock, indexOfInode);
    return written;//if all goes well then size is returned but this is how mny times there was a write made... I hopes
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
    int index;
    char *paths[strlen(file)];
    int depth = BreakDownPathName(file, paths);
    char *filename = calloc(sizeof(char), MAX_FILENAME_LENGTH);
    memcpy(filename, paths[depth - 1], MAX_FILENAME_LENGTH);
    char *parentPath = calloc(sizeof(char), strlen(file)- strlen(filename));
    strncat(parentPath, file, strlen(file)- strlen(filename) - 1);
    int absoluteInodePointer = DoesThisPathExist(parentPath);
    if (absoluteInodePointer == -1) //the file does not exist
    {
        osErrno = E_NO_SUCH_FILE;
        return -1;
    }
    for (index = 0; index < MAX_NUM_OPEN_FILES; index++)
    {
        if (absoluteInodePointer == fileTable[index].inodePointer) //the two strings are the same, therefore this file is in use
        {
            osErrno = E_FILE_IN_USE;
            return -1;
            //directory still needs to be cleared out
        }
    }
    int *dataPointers = calloc(sizeof(int), MAX_NUM_SECTORS_PER_FILE);
    depth = ReadInodeSectors(absoluteInodePointer, dataPointers);//read the files data pointers into the dataPointers buffer
    FreeTableOf(&dataMap, dataPointers, fileTable[index].sizeOfFile);//clear the datamap of these blocks
    FreeTableOfOne(&inodeMap, fileTable[index].inodePointer); //clear the inode
    printf("FS_Unlink\n");
    return 0;
}

// directory ops
int
Dir_Create(char *path)
{
    //this is really similar the other file create
//    char *myPaths = BreakDownPathName(path);//myPaths now contains the paths of the directory with the last being the one to be create
    // we need to make sure that the files before this are real.. use the first values in myPath to find this out
    char *inodeEntry = BuildInode(DIRECTORY_ID); //this builds an empty inode entry

    int absInode;
    char *parents[strlen(path)];
    int depth = BreakDownPathName(path, parents);
    int index;
    int sector;
    char *dirName = malloc(MAX_FILENAME_LENGTH);
    memccpy(dirName, parents[depth - 1], strlen(parents[depth - 1]));
    char *parentPath = calloc(sizeof(char), (strlen(path) - strlen(dirName)));
    strncat(parentPath, path, (strlen(path) - strlen(dirName) - 1));
    int parentInode;
    if (depth == 1)//useless to do anything else, we are on the root
    {
        parentInode = 0;
    }
    else
    {
        parentInode = DoesThisPathExist(parentPath);
    }
    int parentInodeSector = GetSector(parentInode);
    int parentInodeSectorIndex = GetSectorIndex(parentInode);
    char *inode = calloc(sizeof(char), SECTOR_SIZE_1 / NUM_INODES_PER_BLOCK);
    inode = GetInode(parentInodeSector, parentInodeSectorIndex);
    int loc = InsertDirectory(inode, dirName, &dataMap, &inodeMap);//insert a directory on to the disk
    InjectInode(parentInodeSector, inode, parentInodeSectorIndex);//put the inode back in to the disk!
    char *debug = malloc(SECTOR_SIZE);
    Disk_Read(parentInodeSector, debug);
    char x = debug[8];
    //this method puts teh
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
    int *pointers = calloc(sizeof(int), MAX_NUM_SECTORS_PER_FILE);
    int length = ReadInodeSectors(thisInode, pointers); //this function needs to be verified that it returns the actual sectors of data blocks
    //read the data
    char *dataBlock = malloc(SECTOR_SIZE_1 * sizeof(char));
    char *directoryEntries = malloc(DIRECTORY_LENGTH * sizeof(char));
    int index;
    int subIndex;
    int end;
    int start = 0;
    char *strBuf = calloc(sizeof(char), size);
    for (index = 0; index < length; index++)
    {
        //index is each data block
        Disk_Read(pointers[index], dataBlock);
        for (subIndex = 0; subIndex + DIRECTORY_LENGTH < SECTOR_SIZE_1; subIndex+=DIRECTORY_LENGTH)
        {
            //subIndex is the index of the data block we are on in terms of directories
            int count;
            for(count = 0; count < DIRECTORY_LENGTH; count++)
            {
                //write the directory from the data block
                directoryEntries[count] = dataBlock[count + subIndex];
            }
            if (directoryEntries != NULL)//make sure the directory is real
            {
                //concatenate the recently collected directory onto the buffer
                end = start + DIRECTORY_LENGTH;
                for (start; start < end; start++)
                {
                    strBuf[start] = directoryEntries[start - (end - DIRECTORY_LENGTH)];
                }
            }
        }
    }
    memcpy(buffer, strBuf, size); //copy the char pointer to aa void pointer
    free(dataBlock);
    free(pointers);
    free(thisInode);
    //deallocate the memory assigned
    return start / DIRECTORY_LENGTH; //ther iterator serves as the count of directories
}

int
Dir_Unlink(char *path)
{
    char *rootPath = "/"; //this is what the root is
    if (strcmp(path, rootPath) == 0)//cannot delete the root
    {
        osErrno = E_ROOT_DIR;
        return FAILURE;
    }
    if (Dir_Size(path) != 0)
    {
        osErrno = E_DIR_NOT_EMPTY;
        return FAILURE;
    }
    int parentInode;
    int thisInode;//aabsolute inode pointer... ie 768 is the 768th inode which is mapped by inodeMap.bytemap[192] and is the 0th inode on that block.
    //the sector is something like 196 (192 + FirstSectorIndex)
    if (thisInode = DoesThisPathExist(path) == FAILURE)
    {
        osErrno = E_NO_SUCH_FILE;
        return -1;
    }
    FreeTableOfOne(&inodeMap, thisInode);
    //remove the entry from the parent
    char *parentPath;
    char *filenameToRemove[strlen(path)];
    int length = BreakDownPathName(path, filenameToRemove);//the last string is the file name
    strncat(parentPath, path, strlen(path) - DIRECTORY_LENGTH);//should trim the last part of the string
    parentInode = DoesThisPathExist(parentPath);//the inode of the parent
    RemoveDirectory(parentInode, filenameToRemove[length - 1], &dataMap);
    //now delete this inode by opening it up
    //remove the directory from its parent
    //free the inode
    //free the data blocks

    printf("Dir_Unlink\n");
    return 0;
}
//Searches for the first junk fileTableElement on the fileTable
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
