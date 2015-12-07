#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Params.h"
#include "FileTable.h"

#define GARBAGE 0

//an element in the file table
//will hold the inode of the file, its size, and its index!
bool IsEmpty(FileTableElement *passed)
{
    return (passed->fileOpenCount == GARBAGE);
    //is the fileOpen count equal to garbage and threrefore free?
}
void SetToNull(FileTableElement *entry)
{
    *entry = initFileTableElement();
}
FileTableElement initFileTableElement()
{
    FileTableElement ft;
    ft.fileOpenCount = GARBAGE;
    ft.index = GARBAGE;
    ft.inodePointer = GARBAGE;//this may need to be negative
    ft.sizeOfFile = GARBAGE;
    return ft;
}

bool FileTableOpen(FileTableElement *element, int inode, char *filename)
{
    if (element->fileOpenCount == 0)
    {//the file is not open and has no restrictions
        element->inodePointer = inode;
        element->fileOpenCount++;
        GetAndSetSize(element);//get the size of the file
        SetFilename(element, filename); //setting the filename of the file into the file table
        //Setting the filename is need to easily search for the file in the filetable
        return true;
    }
    if (element->inodePointer != inode)
    {
        return false;//this entry is alreaddy in use and the inodes are not the same!
    }
    //the file is open but its inode is the same, therefore just incremnt the open count
    element->fileOpenCount++;
    //the file has been opened!

    GetAndSetSize(element);

    return element->fileOpenCount > 0;
}
bool GetAndSetSize(FileTableElement *element)
{
    //TODO (Sam#2#): get the size of a file
    element->sizeOfFile = 0;
    return true;
}
bool IsGarbage(FileTableElement element)
{
    return element.fileOpenCount == 0;//files are considered garbage or empty if the fileOpenCount is equall to 0
}
bool SetToGarbage(FileTableElement *element)
{
    element->fileOpenCount = GARBAGE;
    return element->fileOpenCount == GARBAGE;
}
int GetLengthOfFile(FileTableElement *element)
{
    return -1;
}
bool FileTableClose(FileTableElement *element)
{
    if (!IsGarbage(*element))
    {
        element->fileOpenCount--;//this file is closed by taking the open count down by one
        return true; //the file wasn't garbage
    }
    return false; //the file was garbage
}
bool SetFilename(FileTableElement *element, char *filename)
{
    if (strlen(filename) < MAX_PATH_LENGTH)//check if the file name is of the correct length
    {
        element->fileName = filename; //set the filename
        return 0;
    }
    else
    {
        //the file name is too long
        return -1;
    }
}
