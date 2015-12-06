#include <stdio.h>
#include <string.h>
#include "Directory.h"
#include "LibFS.h"
#include "Params.h"
void
usage(char *prog)
{
    fprintf(stderr, "usage: %s <disk image file>\n", prog);
    exit(1);
}

int
main(int argc, char *argv[])
{
    char *testDir = malloc(DIRECTORY_LENGTH);
    testDir = BuildDirectoryEntry("a file", 1234);
    printf("%s\n", testDir);
    char *path = "testLib";
    char *makeADirNAME = "\\usr\\";
    char *aFilePath = "\\usr\\fileX\\";
    char *writeData = "A quick brown fox jumped over the lazy dog";
    void *buffer = calloc(sizeof(char), SECTOR_SIZE_1);
    buffer = (char *) writeData;
    FS_Boot(path); //
    printf("Dir_Create returned %d\n", Dir_Create(makeADirNAME));
    printf("File_Create returned %d\n", File_Create(aFilePath));
    int fd = File_Open(aFilePath);
    printf("The fd of the file is: %d\n", fd);
    File_Write(fd, buffer, SECTOR_SIZE_1);
    //free(buffer);
    buffer = calloc(sizeof(char), SECTOR_SIZE_1);
    File_Read(fd, buffer, SECTOR_SIZE_1);
    printf("%s\n", buffer);
    //printf("%s\n", buffer);
//    printf("%d\n", Dir_Size(makeADirNAME)); //should be 20
    FS_Sync();//not sure what this does
    return 0;
}

