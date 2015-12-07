#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED

#include <stdbool.h>
#include "Map.h"

char *BuildDirectoryEntry(char *name, int pointer);
int InsertDirectory(char *inodeOfParent, char *newDirectoryEntry, Map *data, Map *inode);
int GetSize(char *directory);
bool RemoveDirectory(int parentInodeSector, char *filenameToRemove, Map *dataMap);
int BreakDownPathName(char *filename, char *emptyStrArray);
char *getFilename(char *dir);
int GetDirPointer(char *dir);

#endif // DIRECTORY_H_INCLUDED
