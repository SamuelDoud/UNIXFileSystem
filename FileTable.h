#ifndef FILETABLE_H_INCLUDED
#define FILETABLE_H_INCLUDED

typedef struct FileTableElement {
  int inodePointer; //the pointer to the sector of this file's inode
  int indexOfInodeInSector;//only useful in inodes
  int index; //where this file is in terms of index
  int fileOpenCount; //how many times this file has been accessed. No use now
  int sizeOfFile; //how large this file is
  char *fileName; //this is the name of the file
} FileTableElement;
bool IsEmpty(FileTableElement *passed);
static FileTableElement ft;
FileTableElement initFileTableElement();
bool GetAndSetSize(FileTableElement *);
//should be all zeros initily
void SetToNull(FileTableElement *entry);
//checks if the file table element passed is empty

FileTableElement initFileTableElement();
bool FileTableOpen(FileTableElement *element, int inode);
bool IsGarbage(FileTableElement element);
bool SetToGarbage(FileTableElement *element);
int GetLengthOfFile(FileTableElement *element);
bool FileTableClose(FileTableElement *element);
bool SetFileName(FileTableElement *element, char *name);
#endif // FILETABLE_H_INCLUDED
