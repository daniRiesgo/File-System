typedef unsigned char byte; //let's optimize!!

typedef struct {
	char name[32];
	byte counter;
} tag;

/*
inode struct. Info about the file
*/
 typedef struct {
 	char name[64];
 	byte state;
 	short fileSize;
 	unsigned short offset;
 	byte tagId[3];
} inode;

/*
 superblock. What do we need here? think.
*/
 typedef struct {
 	byte numFiles;
 	byte maxNumFiles;
 	tag tags[30];
 	byte tagmap[30];
 } superblock;



 void main() {

 	int sizeSb = sizeof(superblock);
 	int sizeInodes = sizeof(inode) * 50;

 	printf("%d\n", sizeSb);
 	printf("%d\n", sizeInodes);



 }
