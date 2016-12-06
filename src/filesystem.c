/* This file contains the definition of the functions that must be implemented
 * to allow the user access to the file system and the files.
 */

#include "include/filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 // neccesary to check if there is an open file when unmounting the filesystem
#define STATE_OPEN 0
#define STATE_CLOSED 1

#define MAX_FILES 50
#define MAX_NAME_LENGTH 64

#define MAX_TAGS 30
#define MAX_TAGS_FILE 3
#define MAX_TAG_NAME 32
#define OCCUPIED_TAG 1
#define NO_TAG 255
#define FREE_TAG 0

#define MIN_FS_SIZE_KB 320
#define MAX_FS_SIZE_KB 500

typedef unsigned char byte; //let's optimize!!

typedef struct {
	char name[MAX_TAG_NAME];
	byte counter;
} tag;

/*
inode struct. Info about the file
*/
 typedef struct {
 	char name[MAX_NAME_LENGTH];
 	byte state;
 	short fileSize;
 	short offset;
 	byte tagId[MAX_TAGS_FILE];
} inode;

/*
 superblock. What do we need here? think.
*/
 typedef struct {
 	byte numFiles;
 	byte maxNumFiles;
 	tag tags[MAX_TAGS];
 	byte tagmap[MAX_TAGS];
 } superblock;

inode inodes[MAX_FILES];


 superblock sblock;

/***************************/
/* File system management. */
/***************************/

 int saveDataToDisk() {
 	// write in disk the superblock
	char buff[BLOCK_SIZE];
	memcpy(buff, (char*) &sblock, sizeof(superblock));
	if (bwrite(DEVICE_IMAGE, 0, buff) == -1) { return -1; }

	// write in disk the inodes
	memcpy(buff, (char*) &inodes, sizeof(inode) * MAX_FILES);
	if (bwrite(DEVICE_IMAGE, 1, buff) == -1) { return -1; }

	return 0; // success
 }

/*
 * Formats a device.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mkFS(int maxNumFiles, long deviceSize) {

	if (maxNumFiles < 0 || maxNumFiles > MAX_FILES) {
		//error
		return -1;
	}

	if (deviceSize < MIN_FS_SIZE_KB * 1024 || deviceSize > MAX_FS_SIZE_KB * 1024) { return -1; }

	//create FS
	sblock.numFiles = 0;
	sblock.maxNumFiles = maxNumFiles;
	int i;
	for (i = 0; i < MAX_TAGS; i++) { sblock.tagmap[i] = FREE_TAG; }

	// format disk
	char dummy_block[BLOCK_SIZE];
	memset(dummy_block, '0', BLOCK_SIZE);
	long blocks = deviceSize / BLOCK_SIZE;
	for (i = 0; i < blocks; i++) {
		if (bwrite(DEVICE_IMAGE, i, dummy_block) == -1) { return -1; }
	}

    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return 0;
}

/*
 * Mounts a file system from the device deviceName.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int mountFS() {
	//read metadata from disk to memory
	// read the superblock
	char buff[BLOCK_SIZE];
	if (bread(DEVICE_IMAGE, 0, buff) == -1) { return -1; }
	memcpy((char*) &sblock, buff, sizeof(superblock));

	// read the inodes
	if (bread(DEVICE_IMAGE, 1, buff) == -1) { return -1; }
	memcpy((char*) &inodes, buff, sizeof(inode) * MAX_FILES);

	return 0;
}

/*
 * Unmount file system.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int umountFS() {
	// write data from memory to disk
	int i;
	for (i = 0; i < sblock.numFiles; i++) { closeFS(i); }

    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return 0;
}

/*******************/
/* File read/write */
/*******************/

/*
 * Creates a new file, if it doesn't exist.
 * Returns 0 if a new file is created, 1 if the file already exists or -1 in
 * case of error.
 */
int creatFS(char *fileName) {

	//if there is no space for more files
	if (sblock.numFiles >= sblock.maxNumFiles) { return -1;	}

	//if the name of the file is longer than the max or is empty, error
	if (strlen(fileName) > MAX_NAME_LENGTH || strlen(fileName) == 0) {
		return -1;
	}

	int numFiles = sblock.numFiles;
	int i = 0;
	for (i = 0; i < numFiles; i++) {
        // check whether the file already exists
		if (strcmp(inodes[i].name, fileName) == 0) { return 1; }
	}

	//create new inode
	inode file;
	strcpy(file.name, fileName);
	file.state = STATE_CLOSED;
	file.fileSize = 0;
	file.offset = 0;
	for (i = 0; i < MAX_TAGS_FILE; i++) { file.tagId[i] = NO_TAG; }

	//write inode in the array of inodes (extract to a function?)
	memcpy(&inodes[sblock.numFiles], &file, sizeof(inode));
	sblock.numFiles++;

    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return 0;
}

/*
 * Opens an existing file.
 * Returns file descriptor if possible, -1 if file does not exist or -1 in case
 * of error.
 */
int openFS(char *fileName) {

    // error when the filename is an invalid one (too big)
	if (strlen(fileName) > 64) { return -2; }

	int i;
	//iterate over the inodes until the fileName and name coincide
	for (i = 0; i < sblock.numFiles; i++) {
		if (strcmp(inodes[i].name, fileName) == 0) {
            // open the file when found in the filesystem
			if (inodes[i].state == STATE_CLOSED) { inodes[i].state = STATE_OPEN; }
            // write-through metadata to the device from the memory buffer
        	if (saveDataToDisk() == -1) { return -1; }
            // return file descriptor
			return i;
		}
	}

	return -1; // error when the file was not found
}

/*
 * Closes a file.
 * Returns 0 if the operation was correct or -1 in case of error.
 */
int closeFS(int fileDescriptor) {

	// if the filedescriptor is ilegal, error
	if (fileDescriptor < 0 || fileDescriptor > sblock.numFiles) { return -1; }

	// check if the file is closed
	if (inodes[fileDescriptor].state == STATE_CLOSED) { return -1; }

	inodes[fileDescriptor].offset = 0; // reset offset
	inodes[fileDescriptor].state = STATE_CLOSED; // close file

    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return 0; // success
}

/*
 * Reads a number of bytes from a file and stores them in a buffer.
 * Returns the number of bytes read or -1 in case of error.
 */
int readFS(int fileDescriptor, void *buffer, int numBytes) {

    // PARAMETER CHECK

	// check whether the file is open
	if (inodes[fileDescriptor].state == STATE_CLOSED) { return -1; }
	// check whether the number of bytes to be read is legal
	if (numBytes < 0 || numBytes > MAX_FILE_SIZE) { return -1; }

    // READ OPERATION

    // Set the amount of bytes to be read to the maximum available if such is less than the requested ones.
	int sizeToRead = numBytes;
	if (inodes[fileDescriptor].fileSize < numBytes) {
		sizeToRead = inodes[fileDescriptor].fileSize;
	}
    // bring the whole block from device to memory
	char buff[MAX_FILE_SIZE];
    // note: the data blocks start in the 3rd block
	if (bread(DEVICE_IMAGE, 2 + fileDescriptor, buff) == -1) { return -1; }
    // return the requested information via the passed buffer
	int offset = inodes[fileDescriptor].offset;
	memcpy(buffer, &buff[offset], sizeToRead);

    // METADATA UPDATES

    // update file pointer
	inodes[fileDescriptor].offset += sizeToRead;
    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return sizeToRead;
}

/*
 * Reads number of bytes from a buffer and writes them in a file.
 * Returns the number of bytes written, 0 in case of end of file or -1 in case
 * of error.
 */
int writeFS(int fileDescriptor, void *buffer, int numBytes) {

    // PARAMETER CHECK

	// check whether the file is open
	if (inodes[fileDescriptor].state == STATE_CLOSED) { return -1; }
	// check whether the number of bytes to be written is legal
	if (numBytes < 0 || numBytes > MAX_FILE_SIZE) { return -1; }
	// if the offset is equal to MAX_FILE_SIZE, EOF. Returns 0.
	if (inodes[fileDescriptor].offset == MAX_FILE_SIZE) { return 0; }
    // Calculate the space left from the current pointer until the end of the file
	int offset = inodes[fileDescriptor].offset;
	int space = MAX_FILE_SIZE - offset;
    // Check whether the contents wanted to be written fit the remaining block space
	if (space < numBytes) { return -1; }

    // DATA WRITE OPERATION

	// Transfer the block from device to memory buff
	char buff[BLOCK_SIZE];
    // note: the data blocks start in the 3rd block
	if (bread(DEVICE_IMAGE, 2 + fileDescriptor, buff) == -1) { return -1; }
	// transfer the new content to the memory buffer
	memcpy(&buff[offset], buffer, numBytes);
    // write-through data to the device from the memory buffer
    if (bwrite(DEVICE_IMAGE, 2 + fileDescriptor, buff) == -1) { return -1; }

    // METADATA UPDATES

	// update the size of the file when the write op did not only overwrite
	if (inodes[fileDescriptor].fileSize < offset + numBytes) {
		inodes[fileDescriptor].fileSize = offset + numBytes;
	}
	// update the position of the file pointer
	inodes[fileDescriptor].offset += numBytes;
    // write-through metadata to the device from the memory buffer
	if (saveDataToDisk() == -1) { return -1; }

	return numBytes;
}

int _lseek(int whence, int fileDescriptor, long offset) {
    int finalOffset;
    // Pointer update
	switch(whence) {
		case FS_SEEK_END:
			inodes[fileDescriptor].offset = inodes[fileDescriptor].fileSize;
			finalOffset = inodes[fileDescriptor].offset;
			break;
		case FS_SEEK_BEGIN:
			inodes[fileDescriptor].offset = 0;
			finalOffset = inodes[fileDescriptor].offset;
			break;
		case FS_SEEK_SET:
			// if the offset is negative or bigger than the size of the file.
			if (offset < 0 || inodes[fileDescriptor].fileSize < offset + inodes[fileDescriptor].offset) {
				return -1;
			}
			inodes[fileDescriptor].offset += offset;
			finalOffset = inodes[fileDescriptor].offset;
			break;
		default:
			return -1;
	}
    // metadata write-through
    if (saveDataToDisk() == -1) { return -1; }

    return finalOffset;
}


/*
 * Repositions the pointer of a file. A greater offset than the current size, or
 * an offset lower than 0 are considered errors.
 * Returns new position or -1 in case of error.
 */
int lseekFS(int fileDescriptor, long offset, int whence) {

    // PARAMETER CHECK

	if (fileDescriptor < 0 || fileDescriptor > sblock.numFiles) {
		return -1;
	}
	if (inodes[fileDescriptor].state == STATE_CLOSED) {
		return -1;
	}


    // LSEEK OPERATION

	return _lseek(whence, fileDescriptor, offset);;
}

/**********************/
/* Version management */
/**********************/

/*
 * Returns 0 if the file has the tag, -1 if not.
 */
int tagAlreadyInFile(char *fileName, int tagId){
	int i, j;
	for (i = 0; i < sblock.numFiles; i++) {
		if(strcmp(inodes[i].name, fileName) == 0) {
			for (j = 0; j < MAX_TAGS_FILE; j++) {
				if (inodes[i].tagId[j] == tagId) { return 0; }
			}
		}
	}
	return -1;
}

/*
 * Returns the id of the tag if it exists. -1 if error.
 */
int tagExists(char *tagname) {
	int i;
	for (i = 0; i < MAX_TAGS; i++) {
		if (sblock.tagmap[i] == OCCUPIED_TAG) {
			if (strcmp(sblock.tags[i].name, tagname) == 0) { return i; }
		}
	}
	return -1;
}

int getInodeIndexByFileName(char *filename){
	int i;
	for (i = 0; i < sblock.numFiles; i++) {
		if (strcmp(inodes[i].name, filename) == 0) { return i; }
	}
	return -1;
}

/*
 * Return the first free position of the tags array. -1 otherwise.
 */
int getFreeTagIndex(){
	int i;
	for (i = 0; i < MAX_TAGS; ++i) {
		if (sblock.tagmap[i] == FREE_TAG) { return i; }
	}
	return -1;
}

void addTagToSystem(char* tagName, int index){
	strcpy(sblock.tags[index].name, tagName);
	sblock.tagmap[index] = OCCUPIED_TAG;
	sblock.tags[index].counter = 1;
}

/*
 * Returns 0 if the tag is added. -1 if error.
 */
int addTagToFile(int fd, int tagId){
	int i;
	for (i = 0; i < MAX_TAGS_FILE; i++) {
		if(inodes[fd].tagId[i] == NO_TAG){
			inodes[fd].tagId[i] = tagId;
			sblock.tags[tagId].counter++;
			return 0;
		}
	}
	return -1;
}

/*
 * Tags a file with the given tag name. Returns 0 if the operation is
 * successful, 1 if the file already had that tag or -1 in case of error.
 */
int tagFS(char *fileName, char *tagName) {

    // CHECK PARAMETERS

    // Is tag name too long?
	if (strlen(tagName) > MAX_TAG_NAME) { return -1; }

    // Does the filename refer to a real file?
	int fd = getInodeIndexByFileName(fileName);
	if (fd == -1) { return -1; }

    // Is the referred file open?
	if (inodes[fd].state == STATE_OPEN) { return -1; }

    // TAG THE FILE

    // Does the tag already exist?
	int tagId = tagExists(tagName);

	if (tagId == -1) { // when it doesn't, create a new tag
        // get an empty tag position when possible
		int freeId = getFreeTagIndex();
        // error when no free positions left
		if (freeId == -1) { return -1; }
        // tag the file!
		if (addTagToFile(fd, freeId) == -1) { return -1; }
        // register the operation in the superblock
		addTagToSystem(tagName, freeId);

	} else { // when the tag already existed
		// return error if the file already has the tag
		int i;
		for (i = 0; i < MAX_TAGS_FILE; ++i) {
			if(inodes[fd].tagId[i] == tagId) { return 1; }
		}
		// tag the file!
		if(addTagToFile(fd, tagId) == -1) { return -1; }
	}

    // metadata write-through
	if (saveDataToDisk() == -1) { return -1; }

	return 0;
}

/*
 * Removes a tag from a file. Returns 0 if the operation is successful, 1 if
 * the tag wasn't associated to the file or -1 in case of error.
 */
int untagFS(char *fileName, char *tagName) {

    // CHECK PARAMETERS

    // Does the filename refer to an existing file?
	int fd = getInodeIndexByFileName(fileName);
	if (fd == -1) { return -1; }

	if (inodes[fd].state == STATE_OPEN) { return -1; }

    // Is the tag an existing one?
	int tagId = tagExists(tagName);
	if (tagId == -1) { return -1; }

    // UNTAG THE FILE

	int i;
    // look for the tag in the file tag list
	for (i = 0; i < MAX_TAGS_FILE; ++i) {
		if(inodes[fd].tagId[i] == tagId) {
            // remove the tag when found
			inodes[fd].tagId[i] = NO_TAG;
            // decrease the tag links counter
			sblock.tags[tagId].counter--;
            // remove the tag when becomes unused
			if (sblock.tags[tagId].counter == 0) {
				sblock.tagmap[tagId] = FREE_TAG;
			}
            // metadata write-through
        	if (saveDataToDisk() == -1) { return -1; }

			return 0;
		}
	}

    // error when the tag was not found in the file
	return 1;
}

/*
 * Looks for all files tagged with the tag tagName, and stores them in the list
 * passed as a parameter. Returns the number of tagged files found or -1 in
 * case of error.
 */
int listFS(char *tagName, char **files) {
	int i, j;
	int id = tagExists(tagName);
	if (id == -1){
		return -1;
	}
	int count = 0;
	for (i = 0; i < sblock.numFiles; i++) {
		for (j = 0; j < 3; j++) {
			if (inodes[i].tagId[j] == id) {
				// add to list
				strcpy(files[count], inodes[i].name);
				count++;
			}
		}
	}
	return count == 0 ? -1 : count;
}
