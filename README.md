u# UNIXFileSystem
Group final project for CSC-585

Nick Bohay, ________
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

	Recap: 1 sector for superblock, 1 sector for inode block bitmap, 3 sectors for data block bitmap, 63 for inodes. 
	therefore, we have 10000 -1 -1 -3 -250 = 9745 sectors remaining. Since there is a bijection between data blocks and sectors, we have 9745 data blocks remaining. 

Directory
	-a special kind of file denoted by a 1 byte (typo in notes says b) flag. 
	-A many to 1 map between sets of names, and inodes. 
	-format: a fixed 16-byte field for the name, and a 4-byte entry as the inode number.
