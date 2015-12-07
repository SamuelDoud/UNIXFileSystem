CC=gcc

main: Inode.c Directory.c FileTable.c main.c LibDisk.c LibFS.c Map.c Builder.c
	$(CC) -o main *.c
