#ifndef __File_Table_H__
#define __File_Table_H__

#define GARBAGE -1

//an element in the file table
//will hold the inode of the file, its size, and its index!
typedef struct FileTableElement {
  int inodePointer; //the pointer to the sector of this file's inode
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
bool IsGarbage(FileTableElement ft)
{
    return ft.inodePointer == 0;//an inode pointer of zero is garbage since this is the superblock!
}
bool SetToGarbage(FileTableElement *ft)
{
    ft->index = GARBAGE;
    return ft->index == GARBAGE;
}
int GetLengthOfFile(FileTableElement *element)
{
    return -1;
}
#endif // __Disk_H__
