#include <stdio.h>

#include "LibFS.h"
#include "LibDisk.h"

#define MAGIC_NUMBER 8723

char *BuildSuperBlock()
{
    char *SuperBlock = malloc(SECTOR_SIZE * sizeof(char));
    memset(SuperBlock, '\0', SECTOR_SIZE);
    //create an empty and null superblock
    sprintf(SuperBlock, sizeof(SuperBlock), "%d", MAGIC_NUMBER);
    //put the magic number in the superblock and return it

    return SuperBlock;
}

