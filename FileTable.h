#ifndef __File_Table_H__
#define __File_Table_H__

#define GARBAGE 0

//an element in the file table
//will hold the inode of the file, its size, and its index!
typedef struct FileTableElement {
  int inodePointer; //the pointer to the sector of this file's inode
  int indexOfInodeInSector;
  int index; //where this file is in terms of index
  int fileOpenCount; //how many times this file has been accessed. No use now
  int sizeOfFile; //how large this file is
} FileTableElement;
static FileTableElement ft;
FileTableElement initFileTableElement();
//should be all zeros initily
void SetToNull(struct FileTableElement *entry);
//checks if the file table element passed is empty
bool IsEmpty(struct FileTableElement *passed)
{
    return (passed == NULL);
}

void SetToNull(struct FileTableElement *entry)
{
    *entry = initFileTableElement();
}
FileTableElement initFileTableElement()
{
    ft.fileOpenCount = 0;
    ft.index = 0;
    ft.inodePointer = 0;//this may need to be negative
    ft.sizeOfFile = 0;
    return ft;
}
bool FileTableOpen(FileTableElement *element, int inode)
{
    if (element->fileOpenCount == 0)
    {//the file is not open and has no restrictions
        element->inodePointer = inode;
        element->fileOpenCount++;
        return true;
    }
    if (FileTableElement->inodePointer != inode)
    {
        return false;//this entry is alreaddy in use and the inodes are not the same!
    }
    //the file is open but its inode is the same, therefore just incremnt the open count
    FileTableElement->fileOpenCount++;
        //the file has been opened!

    SetSize(element);
}
bool SetSize(FileTableElement *element)
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
    return ft->fileOpenCount == GARBAGE;
}
int GetLengthOfFile(FileTableElement *element)
{
    return -1;
}
bool FileTableClose(FileTableElement *element)
{
    if (!IsGarbage(elment))
    {
        element.fileOpenCount--;//this file is closed by taking the open count down by one
        return true; //the file wasn't garbage
    }
    return false; //the file was garbage
}
#endif // __Disk_H__
