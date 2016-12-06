#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/filesystem.h"

// #define ANSI_COLOR_RESET   "\x1b[0m"
// #define ANSI_COLOR_RED     "\x1b[31m"
// #define ANSI_COLOR_GREEN   "\x1b[32m"

#define SUCCESS             "\x1b[32mSUCCESS\n\x1b[0m"
#define FAIL                "\x1b[31mFAILED\n\x1b[0m"

int fs_tests(){

    int result;

    fprintf(stdout, "%s", "1. FYLESYSTEM TESTS:\n");

    // 1. Testing the creation of a filesystem using adequate values
    fprintf(stdout, "%s", "\t1.1. TEST mkFS - correct creation: ");
    result = mkFS(50, 374800);

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.1 Create a filesystem with more capacity than available storage (returns -1)
    fprintf(stdout, "%s", "\t\t1.1.1 TEST mkFS - not enough storage: ");
    result = mkFS(50, 37480000);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 2. Mount the device correctly
    fprintf(stdout, "%s", "\t1.2. TEST mountFS - correct mount: ");
    result = mkFS(50, 374800);
    result = mountFS();

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // // 2.1. Mount the device already mounted
    // fprintf(stdout, "%s", "\t\t1.2.1. TEST mountFS - already mounted (-1): ");
    // result = mountFS();
    //
	// if(result == -1) {
	// 	fprintf(stdout, "%s", SUCCESS);
	// } else {
    //     fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    // }

    // 3. Unmount the device correctly
    fprintf(stdout, "%s", "\t1.3. TEST umountFS - correct unmount: ");
    result = umountFS();

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // // 3.1. Mount the device already mounted
    // fprintf(stdout, "%s", "\t\t1.3.1. TEST umountFS - already unmounted (-1): ");
    // result = umountFS();
    //
	// if(result == -1) {
	// 	fprintf(stdout, "%s", SUCCESS);
	// } else {
    //     fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    // }

    return 0;
}

int files_tests(){

    int result, fd, i;
    char buffer[BLOCK_SIZE];
    char str[2];
    char *teststr = "hello world!";

    fprintf(stdout, "%s", "2. FILES TESTS: \n");

    // 1. Testing the creation of a file using adequate values
    fprintf(stdout, "%s", "\t2.1. TEST creatFS - correct creation: ");
    result = mkFS(50, 374800);
    result = creatFS("test.txt");

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.1 Testing the creation of a file already created
    fprintf(stdout, "%s", "\t\t2.1.1. TEST creatFS - already created: ");
    result = creatFS("test.txt");

	if(result == 1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.2 Testing the creation of a file using a name too long
    fprintf(stdout, "%s", "\t\t2.1.2. TEST creatFS - Name is too long: ");
    result = creatFS("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed "\
    "do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "\
    "minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex "\
    "ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "\
    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "\
    "cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id "\
    "est laborum.txt");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.3 Testing the creation of a file using no name
    fprintf(stdout, "%s", "\t\t2.1.3. TEST creatFS - There is no name: ");
    result = creatFS("");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.3 Testing the creation of a file using no name
    fprintf(stdout, "%s", "\t\t2.1.4. TEST creatFS - More files than possible: ");

    for(i = 0; i<51; i++){
        sprintf(str, "%d", i);
        result = creatFS(str);
    }

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 2. Testing the opening of a file
    fprintf(stdout, "%s", "\t2.2. TEST openFS - correct value: ");
    result = mkFS(50, 374800);
    result = creatFS("test.txt");
    result = openFS("test.txt");
    closeFS(result);

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 2.2 Testing the opening of a file that doesn't exist
    fprintf(stdout, "%s", "\t\t2.2.2. TEST openFS - no file: ");
    result = openFS("fakefile.txt");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3. Testing the closing of a file
    fprintf(stdout, "%s", "\t2.3. TEST closeFS - correct value: ");
    fd = openFS("test.txt");
    result = closeFS(fd);

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3.1 Testing the closing of a file already closed
    fprintf(stdout, "%s", "\t\t2.3.1. TEST closeFS - already closed: ");
    fd = openFS("test.txt");
    closeFS(fd);
    result = closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3.2 Testing the closing of a file that does not exist
    fprintf(stdout, "%s", "\t\t2.3.2. TEST closeFS - no file: ");
    fd = 1234;
    result = closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 4. Testing write a file
    fprintf(stdout, "%s", "\t2.4. TEST writeFS - correct value: ");
    fd = openFS("text.txt");
    result = writeFS(fd, buffer, BLOCK_SIZE);
    closeFS(fd);

	if(result == BLOCK_SIZE) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 4.1. Testing write out of a block
    fprintf(stdout, "%s", "\t\t2.4.1. TEST writeFS - write more than a BLOCK_SIZE: ");
    fd = openFS("text.txt");
    result = writeFS(fd, buffer, 2*BLOCK_SIZE);
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 5. Testing read a file
    fprintf(stdout, "%s", "\t2.5. TEST readFS - correct value: ");
    fd = openFS("text.txt");
    result = readFS(fd, buffer, BLOCK_SIZE);
    closeFS(fd);

	if(result == BLOCK_SIZE) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 5.1. Testing read a file
    fprintf(stdout, "%s", "\t\t2.5.1. TEST readFS - out of the file: ");
    fd = openFS("text.txt");
    result = readFS(fd, buffer, 2*BLOCK_SIZE);
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 5.1. Testing read a file
    fprintf(stdout, "%s", "\t\t2.5.2. TEST readFS - out of the file (negative): ");
    fd = openFS("text.txt");
    result = readFS(fd, buffer, -1*BLOCK_SIZE);
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 6. Testing lseek

    fprintf(stdout, "%s", "\t2.6. TEST lseekFS:\n");

    fprintf(stdout, "%s", "\t\t2.6.1. TEST lseekFS (begin) - correct: ");
    result = mkFS(50, 374800);
    mountFS();
    result = creatFS("text.txt");
    fd = openFS("text.txt");
    result = writeFS(fd, teststr, strlen(teststr));
    result = lseekFS(fd, 0, FS_SEEK_BEGIN);
    closeFS(fd);

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t2.6.2. TEST lseekFS (end) - correct: ");
    fd = openFS("text.txt");
    result = writeFS(fd, teststr, strlen(teststr));
    result = lseekFS(fd, 0, FS_SEEK_END);
    closeFS(fd);

	if(result == strlen(teststr)) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t2.6.3. TEST lseekFS (set) - correct: ");
    fd = openFS("text.txt");
    result = writeFS(fd, teststr, strlen(teststr));
    result = lseekFS(fd, 0, FS_SEEK_BEGIN);
    result = lseekFS(fd, strlen(teststr), FS_SEEK_SET);
    closeFS(fd);

	if(result == strlen(teststr)) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t2.6.4. TEST lseekFS (set) - incorrect: ");
    fd = openFS("text.txt");
    result = writeFS(fd, teststr, strlen(teststr));
    result = lseekFS(fd, strlen(teststr), FS_SEEK_SET);
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t2.6.5. TEST lseekFS (set) - negative: ");
    fd = openFS("text.txt");
    result = writeFS(fd, teststr, strlen(teststr));
    result = lseekFS(fd, -2, FS_SEEK_SET);
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // fd, offset,


    return 0;
}

int tags_tests(){

    int result, fd, i;
    char str[2];
    char **files = (char**) malloc(50*sizeof(char*));
    for(i = 0; i < 50; ++i) {
		files[i] = (char*) malloc(64*sizeof(char));
	}

    fprintf(stdout, "%s", "3. TAGS TESTS: \n");

    // 1. Testing the creation of a tag using adequate values
    fprintf(stdout, "%s", "\t3.1. TEST tagFS - correct creation: ");
    result = mkFS(50, 374800);
    result = mountFS();
    result = creatFS("test.txt");
    result = tagFS("test.txt", "example");

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.1 Testing the tag of a file already tagged
    fprintf(stdout, "%s", "\t\t3.1.1. TEST tagFS - already tagged: ");
    result = tagFS("test.txt", "example");

	if(result == 1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.2 Testing the creation of a tag too long
    fprintf(stdout, "%s", "\t\t3.1.2. TEST tagFS - tag too long: ");
    result = tagFS("test.txt", "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed "\
    "do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "\
    "minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex "\
    "ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "\
    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "\
    "cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id "\
    "est laborum");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 1.4 Testing the creation of a tag in open file
    fprintf(stdout, "%s", "\t\t3.1.3. TEST tagFS - tagging an open file: ");
    fd = openFS("test.txt");
    result = tagFS("test.txt", "tag");
    closeFS(fd);

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 2. Testing the creation of multiple tags in a file
    fprintf(stdout, "%s", "\t3.2. TEST tagFS - more than 1 tag in a file: ");
    creatFS("test2.txt");
    result = tagFS("test2.txt", "tag1");
    result = tagFS("test2.txt", "tag2");

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 2.1. Testing the creation of multiple tags in a file
    fprintf(stdout, "%s", "\t\t3.2.1. TEST tagFS - more than 3 tags in a file: ");
    result = tagFS("test2.txt", "tag3");
    result = tagFS("test2.txt", "tag4");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t3.2.2. TEST tagFS - more than 30 tags: ");
    mkFS(50, 374800);
    for(i = 0; i<30; i++){
        sprintf(str, "%d", i);
        creatFS(str);
        tagFS(str, str);
    }

    creatFS("31");
    result = tagFS("31", "This should break");

	if(result == -1) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t\t3.2.3. TEST tagFS - rewriting tags: ");
    untagFS("1", "1");
    result = tagFS("1", "This should work");

	if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3. Testing the search:
    fprintf(stdout, "%s", "\t3.3. TEST listFS - returns correct: ");
    mkFS(50, 374800);
    mountFS();
    creatFS("correctFile1.txt");
    creatFS("correctFile2.txt");
    creatFS("incorrectFile.txt");
    tagFS("correctFile1.txt", "correct");
    tagFS("correctFile2.txt", "correct");
    tagFS("correctFile1.txt", "incorrect");
    tagFS("incorrectFile.txt", "incorrect");

    result = listFS("correct", files);

	if(result == 2 && strcmp(files[0], "correctFile1.txt") == 0) {
        fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3. Testing the search, no tag created before:
    fprintf(stdout, "%s", "\t\t3.3.1. TEST listFS - inexistent tag: ");
    result = listFS("nothinghere", files);

    if(result == -1) {
        fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 4. Testing untag, correct values:
    fprintf(stdout, "%s", "\t3.4. TEST untagFS - correct value: ");
    creatFS("test3.txt");
    result = tagFS("test3.txt", "tag");
    result = untagFS("test3.txt", "tag");

    if(result == 0) {
        fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 4.1. Testing untag, incorrect values:
    fprintf(stdout, "%s", "\t\t3.4.1. TEST untagFS - file is not tagged: ");
    creatFS("test2.txt");
    tagFS("test2.txt", "tag");
    result = untagFS("test3.txt", "tag");

    if(result == 1) {
        fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 4.2. Testing untag, incorrect values:
    fprintf(stdout, "%s", "\t\t3.4.2. TEST untagFS - file does not exist: ");
    result = untagFS("no-file.txt", "tag");

    if(result == -1) {
        fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3. Testing the search:
    fprintf(stdout, "%s", "\t3.5. TEST listFS + untagFS - returns correct: ");
    mkFS(50, 374800);
    mountFS();
    creatFS("correctFile1.txt");
    creatFS("correctFile2.txt");
    creatFS("incorrectFile.txt");
    tagFS("correctFile1.txt", "correct");
    tagFS("correctFile2.txt", "correct");
    tagFS("correctFile1.txt", "incorrect");
    tagFS("incorrectFile.txt", "incorrect");
    untagFS("correctFile1.txt", "correct");

    result = listFS("correct", files);

    if(result == 1 && strcmp(files[0], "correctFile2.txt") == 0) {
        fprintf(stdout, "%s", SUCCESS);
    } else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    // 3. Testing the search:
    fprintf(stdout, "%s", "\t\t3.5.1. TEST listFS + untagFS all files: ");
    mkFS(50, 374800);
    mountFS();
    creatFS("correctFile1.txt");
    creatFS("correctFile2.txt");
    creatFS("incorrectFile.txt");
    tagFS("correctFile1.txt", "correct");
    tagFS("correctFile2.txt", "correct");
    tagFS("correctFile1.txt", "incorrect");
    tagFS("incorrectFile.txt", "incorrect");
    untagFS("correctFile1.txt", "correct");
    untagFS("correctFile2.txt", "correct");

    result = listFS("correct", files);

    if(result == -1) {
        fprintf(stdout, "%s", SUCCESS);
    } else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    return 0;
}

int persistence_tests(){

    int result, result2, fd, i;
    char **files = (char**) malloc(50*sizeof(char*));
    for(i = 0; i < 50; ++i) {
		files[i] = (char*) malloc(64*sizeof(char));
	}
    char *teststr = "hello world!";

    fprintf(stdout, "%s", "4. PERSISTENCE TESTS: \n");

    // 1. Testing the creation of a tag using adequate values
    fprintf(stdout, "%s", "\t4.1. Correctly written: ");
    result = mkFS(50, 374800);
    result = mountFS();
    result = creatFS("test.txt");
    result2 = tagFS("test.txt", "persistent");
    fd = openFS("test.txt");
    result2 = writeFS(fd, teststr, strlen(teststr));

	if(result == 0 && result2 == strlen(teststr)) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d, %d%s", FAIL, "Value returned by failed test: ", result, result2, "\n");
    }

    fprintf(stdout, "%s", "\tUnmounting... ");
    result = closeFS(fd);
    result = umountFS();

    if(result == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "4.3 Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t4.2 Correctly opened again: ");
    result = mountFS();
    fd = openFS("test.txt");

    if(fd == 0) {
		fprintf(stdout, "%s", SUCCESS);
	} else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    fprintf(stdout, "%s", "\t4.3 Metadata persistence: ");
    result = closeFS(fd);
    result = listFS("persistent", files);

    if(result == 1 && strcmp(files[0], "test.txt") == 0) {
        fprintf(stdout, "%s", SUCCESS);
    } else {
        fprintf(stdout, "%s%s%d%s", FAIL, "Value returned by failed test: ", result, "\n");
    }

    return 0;
}

int main() {

    fs_tests();

    printf("\n\n");

    files_tests();

    printf("\n\n");

    tags_tests();

    printf("\n\n");

    persistence_tests();

	return 0;
}
