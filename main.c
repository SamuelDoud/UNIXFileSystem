#include <stdio.h>
#include <string.h>
#include "Directory.h"
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
    char *path = "testLib";

    FS_Boot(path);
    FS_Sync();//not sure what this does this do?
    return 0;
}

