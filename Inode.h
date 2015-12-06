#ifndef INODE_H_INCLUDED
#define INODE_H_INCLUDED

//Definitions for the inode related functions

bool InjectInode(char *inodeBlock, char *thisInodeData, int index);//put an inode into a inode block based on the index passed
int ReadInodeSectors(char *thisInodeData, int *pointersBuffer);//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
char *AddPointer(char *thisInodeData, int pointerToAdd);//adds the pointerToAdd to the inode data. Returns false on Full. Updates size as well
int SizeOfInode(char *thisInodeData); //return the size of the inode
int GetSectorAt(char *thisInodeData, int index);
int GetParentInodes(int *pointers, int originInode); //pass a int buffer and an origin inode, return the length
char *GetInode(int sector, int index);//get an inode from the disk
char *GetInode(int absPointer);
#endif // INODE_H_INCLUDED
