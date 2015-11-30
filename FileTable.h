#ifndef __File_Table_H__
#define __File_Table_H__


typedef struct FileTableElement {
  int inodePointer;
  int index;
  int fileOpenCount;
  int sizeOfFile;
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
    return ft;
}
#endif // __Disk_H__
