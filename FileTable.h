#ifndef __File_Table_H__
#define __File_Table_H__


typedef struct FileTableElement {
  int inodePointer;
  int index;
  int fileOpenCount;
  int sizeOfFile;
} FileTableElement;
//should be all zeros initily
void SetToNull(struct FileTableElement *entry);
//checks if the file table element passed is empty
bool IsEmpty(struct FileTableElement *passed)
{
    struct FileTableElement emptyElement;
    SetToNull(emptyElement);
    return emptyElement == passed;
}

void SetToNull(struct FileTableElement *entry)
{
    entry = NULL;
}

#endif // __Disk_H__
