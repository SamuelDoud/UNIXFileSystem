#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED

char *BuildDirectoryEntry(char *name, int pointer);
char *InsertDirectory(char *data, char *entry);
int GetSize(char *directory);

#endif // DIRECTORY_H_INCLUDED
