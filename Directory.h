#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED

char *BuildDirectoryEntry(char *name, int pointer);
char *InsertDirectory(char *data, char *entry);
int GetSize(char *directory);
bool RemoveDirectory(char *name, Map *inodes, Map *data);


#endif // DIRECTORY_H_INCLUDED
