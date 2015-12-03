#ifndef INODE_H_INCLUDED
#define INODE_H_INCLUDED

//Definitions for the inode related functions

bool InjectInode(char *inodeBlock, char *thisInodeData, int index);//put an inode into a inode block based on the index passed
int ReadInodeSectors(char *thisInodeData, int *pointersBuffer);//take an inode and read out its pointers to a buffer returning an integer that is the size of the buffer
bool AddPointer(char *thisInodeData, int pointerToAdd);//adds the pointerToAdd to the inode data. Returns false on Full. Updates size as well
int SizeOfInode(char *thisInodeData); //return the size of the inode
int GetSectorAt(char *thisInodeData, int index);

#endif // INODE_H_INCLUDED
