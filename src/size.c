#include <stdio.h>

typedef unsigned char byte; //let's optimize!!

 typedef struct {
 	char name[64];
 	byte state;
 	short fileSize;
 	unsigned short offset;
 } inode;

 typedef struct {
 	unsigned short magicNum;
 	long deviceSize; //neccesary?
 	byte inodesNum;
 	inode inodes[50];
 	byte inodemap[50];
 	byte numFiles;
 	// ...
 } superblock;

 void main(){
 	printf("%lu\n", sizeof(inode));
 	printf("%lu\n", sizeof(byte));
 	printf("%lu\n", sizeof(superblock));

 }