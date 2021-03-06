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
    char test = testDir[18];
    printf("%s\n", testDir);
    char *path = "testLib";
    char *makeADirNAME = "/usr/";
    char *secondDir = "/usr/nextDir/";
    char *makeAfile = "/usr/test/";
    char *aFilePath = "/usr/nextDir/fileX/";
    char *writeData = "A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. A quick brown fox jumped over the lazy dog. !!!!";
    void *buffer = calloc(sizeof(char), strlen(writeData));
    buffer = (char *) writeData;
    FS_Boot(path); //
    printf("Dir_Create returned %d\n", Dir_Create(makeADirNAME));
    printf("The dir %s has been created with result %d\n", secondDir, Dir_Create(secondDir));
    printf("File_Create returned %d\n", File_Create(makeAfile));
    printf("File_Create returned %d\n", File_Create(aFilePath));
    int fd = File_Open(aFilePath);
    int fd2 = File_Open(makeAfile);
    printf("The fd of the file is: %d\n", fd);
    File_Write(fd, buffer, strlen(writeData));
    //free(buffer);
    void *readBuffer = calloc(sizeof(char), strlen(writeData));
    File_Read(fd, readBuffer, strlen(writeData));
    printf("%s\n", readBuffer);
    File_Close(fd);
    File_Close(12);
    File_Unlink(makeAfile);
    void *dirReadout = malloc(1000);
    int size = 1000;
    Dir_Read(makeADirNAME, dirReadout, size);
    printf("Dir_Read: %s\n", dirReadout);
    //printf("%s\n", buffer);
//    printf("%d\n", Dir_Size(makeADirNAME)); //should be 20
    FS_Sync();//not sure what this does
    return 0;
}

