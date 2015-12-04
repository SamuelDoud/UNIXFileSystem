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
    /*
    if (argc != 2) {
    usage(argv[0]);
    }
    char *path = argv[1];
    */
    char *file = "\\usr\\test\\someFile.txt";
    const char s[2] = "\\";
    char *results[strlen(file)];
    int length;
    length =   BreakDownPathName(file, &results);
    printf("%s\n", results[length - 1]);
    return(0);
//    FS_Boot(path);
    FS_Sync();//not sure what this does this do?
    return 0;
}

