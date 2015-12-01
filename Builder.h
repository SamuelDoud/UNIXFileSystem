#ifndef BUILDER_H_INCLUDED
#define BUILDER_H_INCLUDED
#include <stdbool.h>

bool BuildSuperBlock(char *data);
char *BuildDataBlock();
char *BuildInode(int fileType);

#endif // BUILDER_H_INCLUDED
