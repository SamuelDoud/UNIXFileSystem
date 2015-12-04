#include <stdio.h>

#include "LibFS.h"

void
usage(char *prog)
{
    fprintf(stderr, "usage: %s <disk image file>\n", prog);
    exit(1);
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
	usage(argv[0]);
    }
    char *path = argv[1];

    char *aPath = "/usr/atest/somefile.name";

    strtok(aPath, "\\");
    //check how this breaks down

    int index;
    for (index = 0; index < 1; index++)
    {

    }

    FS_Boot(path);
    FS_Sync();//not sure what this does this do?
    return 0;
}

