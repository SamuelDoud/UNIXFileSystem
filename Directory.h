#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED

#include <stdbool.h>
#include "Map.h"

char *BuildDirectoryEntry(char *name, int pointer);
char *InsertDirectory(char *data, char *entry);
int GetSize(char *directory);
bool RemoveDirectory(char *name, Map *inodes, Map *data);
int BreakDownPathName(char *filename, char *emptyStrArray);

#endif // DIRECTORY_H_INCLUDED
