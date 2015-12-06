#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef struct Map{
    int firstSectorIndex;
    int bitsPerChar;
    int length;
    int full;
    char *bytemap;
} Map;

bool FreeTableOfOne(Map *, int);
Map InodeMap();
Map DataMap();
int FindFirstOpenAndSetToClosed(Map *mapArg);
int IndexOfFirstZero(int n, int b);
bool FreeTableOf(Map *mapArg, int *pointers, int lengthOfArray);
bool FreeTableOfOne(Map *mapArg, int pointer);
int intPow(int base, int num);
int GetSector(int absoluteSector);
int GetSectorIndex(int absoluteSector);

#endif // MAP_H_INCLUDED
