#ifndef __File_Table_H__
#define __File_Table_H__

typedef struct FileTableElement {
  int inodePointer;
  int index;
  int fileOpenCount;
  int sizeOfFile;
} FileTableElement;
//should be all zeros initily

bool SetToNull(FileTableElement *entry)
{
    entry = NULL;
}
#endif // __Disk_H__
