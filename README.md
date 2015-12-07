u# UNIXFileSystem
Group final project for CSC-585

Nick Bohay, 4151111
Samuel Doud, 3728396
Evan Russenberger-Rosica, 3401093

In your README file you should have the following five sections:
	-The names and id for all team members.
	-A brief description of how you divided the work between you.
	-Design overview: A few simple paragraphs describing the overall structure of your code and
	any important structures and algorithms. For example, include a brief description of the data structures you use to map between addresses and memory objects, a brief description of the policy that use to perform allocations (e.g., first-fit, best-fit, rotating first-fit, buddy, etc).
	-Known bugs or problems: A list of any features that you did not implement or that you know are not working correctly


Logic:

Sector Allocation Rationale

	-We're given the max. no. of directories and files is 1k. 
		-recall directories are just special kinds of files
		-since there is a bijection between inodes and files, we need 1k inodes.
			-since each inode is represented by a bit in the inode bitmap, we need 1kb = 125B to store this 
			-since each sector is 512B, 1 sector is enough for the inode bitmap
	-We're given the max no. of sectors is 10K
		-subtract 1 for superblock, and 1 for inode bitmap block.
		-subtract 3 for datablock bitmap as 3*512B*8 = 12,228b
			-we know the datablock bitmap needs 3 sectors because:
				-there is a bijection between datablocks and sectors, and we know we have <10k sectors.
					-since each sector is represented by 1 bit, we may need up to 10k bits. 
						-3 sectors of 512*8 bits each are thus required to supply at least 10k bits. 

	Inode Structure:
		-Needs to track file size, and file type (normal vs directory), each represented by an int.
		-need to track which file blocks are allocated to the file. 
			-We're give that the maximum file size is 30 sectors or blocks (since bijction between sectors and data blocks), thus we need to keep 30 disk pointers in each inode, each of which is an int. 
				-therefore each inode is a max of 32Ints, where each int is 4B, therefore each inode has a size of 32B*4B=128B, while each sector is 512B, meaning we can fit 512B/128B = 4 inode blocks/sector. 
					-now since there is a bijection between inodes and files, and we're told we can have max of 1000 files, 
					we can need up to 1000 inodes. We now solve the linear equation 4 inodes/sector * x sectors = 1000inodes = 250, therefore we need 250 sectors to store a maximum of 1000 inodes for our 1000 files.
		-Conclusion: 
			-contains 32 ints (4B each), 1st int is for file size (initialized to 0), 2nd int for type (directory vs file), then 30 pointers, which can point to blocks. 

	Recap: 1 sector for superblock, 1 sector for inode block bitmap, 3 sectors for data block bitmap, 63 for inodes. 
	therefore, we have 10000 -1 -1 -3 -250 = 9745 sectors remaining. Since there is a bijection between data blocks and sectors, we have 9745 data blocks remaining. 


Map Struct: 
	-defines the data sturcutr for both inode and data bitmaps. 

Inode Bitmap:
	-4 bits

Data bimap:

Function Behavior Sketches in FILE API:

	int File Create(char *file)
		- Look if exist/parent exist/etc. if exist fail. if parent doesnt, fail.
		-need to create inode to preserve bijection, done via 
		*BuildInode(int index, int type)
		-modify inode-bitmap, directory (inode + data) accordingly. 
	int File Open(char *file)
		-modify open file table (will include lookup for inode num)
		-return file handle (entry num in table)
	Read(file discriptor, buffer, size)
		-Look in opentable, if entry is valid
		-get file pointer (reports current location in file) from file table.
		-from file pointer calculate data block, read inode, and get number from inode
		-Read sector
		-find location in sector
		-copy from that location upto either EOF or size (into given buffer)
		-If exceeds sector, read next sector from disk and continue
	int File_Write(int fd, void *buffer, int size)
		-Like read:
			-check if filesize + size < 512*30. 

			- Look in opentable, if entry is valid
			- get file pointer (reports current location in file) from file table
			- from file pointer calculate which of the files 0<x<30 data blocks we're in, then find the sector #.
			-read this sector in (copied to a buffer), seek to the next open bit, and begin writing. (repeat until write completed)
			-if we need to allocate new sector<30, call "FindFirstOpenAndSetToClosed", if this returns -1 filesystem full. w
			Write returns -1 and set osErrno to E NO SPACE. 
			-copy to sector.
			-disk_write sector
	int File Seek(int fd, int o↵set) 
		-file needs to be open
		-change value of the pointer in the open file table
	int File Close(int fd)
		-file needs to be open
		-remove the file from the open file table (or decrease counter, approach dependent)
	int File Unlink(char *file)
		-file needs to be closed. (iterate open file table)
		-modify directory by removing entry
		-remove bit from bitmap. 

Function Behavior Sketches in Genric File System API:

	int FS_Boot(char *path):
		-should be called exactly once before any other LibFS functions are called. It takes a single argument, the path, which either points to a real file where your ”disk image” is stored or to a file that does not yet exist and which must be created to hold a new disk image. Upon success, return 0. Upon failure, return -1 and set osErrno to E GENERAL.
		
		-needs to call Disk_Init() OR Disk_load() depending if file exist.
		-If initializing then create bitmaps, magic no, inode for /. inode bitmap init to 0111111. 


	int FS Sync()
		............ 


Misc Function Behavior Sketches:

	Main Function Sketch()
		-Call FS_Boot

	BuildInode(int index, int type):
		-first, call allocate_inode to find empty index. We pass this to buildInode when called
		Creating inodes via int *BuildInode(int index, int type):
			-called by File_Create (need to then create inode to preserve bijection)
			-this creates empty inodes, since when file is created, no data is written. 
			-type specifies if this is a file or directory. 
			-index is the number of the inode, ie 129, passed as param
				-note we need to convert from the index to the block #, and hole or offset in said block. in this example, we'd be in the 129/32 =4th inode block, and the 129%4 = 1st inode in said block. 

			-call File_Read on inote block
				-read file into some buffer

			-call File_Write: 
				-Write first int after offset (repreenting file size), init to 0
				-write file type, (given in parameter)
				-write 30 pointers to blocks (initialized to -1 or ignored)
			-disk_write() 
				-note we read as we do, because we can only write whole blocks, and each inode is less than a block. 

	int FindFirstOpenAndSetToClosed(Map *mapArg) 
		*goes through our bitmaps to find the first free bit. 
			-when this is found, we allocate 128B of memory at this spot. else error. 
			-need way to go from bitmap entry to physical location. 
			-Q: how to write to x/4th of a block?
				A: disk read, seek, write? This way we write whole sectors. 
			-Q: How to set pointers.
				A: we set pointers when we write to files



Function Behavior Sketches in Directory API:

	parent_checker(char *path)
		*checks if the parent of the directory (path) being created exists
		-

	int Dir_Create(char *path)
		*creates a new directory as named by absolute path.
		-check if parent exists
			if not return -1 and set osErrno to E_CREATE.

		-call FindFirstOpenAndSetToClosed to find an open space for our inode. 
		-create an inode in this space with BuildInode. 


		-add a new directory entry in the current directory’s parent. 
		-Upon failure of any sort, return -1 and set osErrno to E_CREATE. Upon success, return 0. Note that for simplicity Dir Create() is not recursive - that is, if only ”/” exists, and you want to create a directory ”/a/b/”, you must first create ”/a”, and then create ”/a/b”.





	int Dir_Size(char *path)
		-returns the number of bytes in the directory referred to by path
		-used to find the size of the directory before calling Dir_Read() (described below) to find the contents of the directory.

	int Dir Read(char *path, void *bu↵er, int size) 
		-used to read the contents of a directory.
		-should return in the buffer a set of directory entries. Each entry is of size 20 bytes, and contains 16-byte names of the directories and files within the directory named by path, followed by the 4-byte integer inode number.
		-If size is not big enough to contain all of the entries, return -1 and set osErrno to E BUFFER TOO SMALL. Otherwise, read the data into the bu↵er, and return the number of directory entries written.

	int Dir Unlink(char *path) 
		-int Dir Unlink(char *path) removes a directory referred to by path, freeing up its inode and data blocks, and removing its entry from the parent directory. Upon success, return 0. Note: Dir Unlink() should only be successful if there are no files or directories within the directory. If there are still files within the directory, return -1 and set osErrno to E DIR NOT EMPTY. If someone tries to remove the root directory (”/”), don’t allow it!!! Return -1 and set osErrno to E ROOT DIR.


Q: What are directories?
	-directories are a special kind of file denoted by a 1 byte (typo in notes says b) flag. 
	-A many to 1 map between sets of names, and inodes. 
	-format: a fixed 16-byte field for the name, and a 4-byte entry as the inode number.
	-Each entry in directory has a total size of 20 bytes. This is made up of 16-byte names of the directories and files within the directory named by path, followed by the 4-byte integer inode number.
	Useful sites:
		http://teaching.idallen.com/dat2330/04f/notes/links_and_inodes.html
		http://teaching.idallen.com/cst8207/13w/notes/450_file_system.html#path-traversal




Questions: 
	Do we need 2 makefiles?

NOTES ON CODE:
	BreakDownPathName(path); commented out in Dir_Create. Need this to check if PIPD (= Parent inode of parent directory) exists. 
