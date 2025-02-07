#include <iostream>
#include <string>
#include <unistd.h>
#include <fuse.h>
#include <fuse/fuse.h>
#include "../libWad/Wad.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

// get_attr
static int my_getattr(const char* path, struct stat* st) {
    memset(st, 0, sizeof(struct stat));

    if (((Wad*)fuse_get_context()->private_data)->isDirectory(path)) {
        st->st_mode = S_IFDIR | 0777;
        st->st_nlink = 2;
        return 0;
    }
    if (((Wad*)fuse_get_context()->private_data)->isContent(path)) {
        st->st_mode = S_IFREG | 0777;
        st->st_nlink = 1;
        st->st_size = ((Wad*)fuse_get_context()->private_data)->getSize(path);
        return 0;
    }
    return -ENOENT;
}

// mknod
static int my_mknod(const char* path, mode_t mode, dev_t rdev) {
    ((Wad*)fuse_get_context()->private_data)->createFile(path);
    return 0;
}

// mkdir
static int my_mkdir(const char* path, mode_t mode) {
    ((Wad*)fuse_get_context()->private_data)->createDirectory(path);
    return 0;
}

// read
static int my_read(const char* path, char* buffer, size_t size, off_t offset, struct fuse_file_info* fi) {
    if (((Wad*)fuse_get_context()->private_data)->isContent(path)) {
        char* buff = new char[((Wad*)fuse_get_context()->private_data)->getSize(path)];
        int returnVal = ((Wad*)fuse_get_context()->private_data)->getContents(path, buff, size, offset);
        memcpy(buffer, buff, returnVal);
        return returnVal;
    }
    else {
        return -ENOENT;
    }
}


// write
static int my_write(const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info* info) {
    return ((Wad*)fuse_get_context()->private_data)->writeToFile(path, buffer, size, offset);
}

// readdir
static int my_readdir(const char* path, void* buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
    (void) offset;
    (void) fi;

    filler(buff, ".", NULL, 0);
    filler(buff, "..", NULL, 0);

    vector<string> fileNames;

    ((Wad*)fuse_get_context()->private_data)->getDirectory(path, &fileNames);

    // loop through the name vector
    for (int i = 0; i < fileNames.size(); i++) {
        filler(buff, fileNames[i].c_str(), NULL, 0);
    }

    return 0;
}


static struct fuse_operations operations = {
    .getattr	= my_getattr,
    .mknod		= my_mknod,
    .mkdir		= my_mkdir,
    .read		= my_read,
    .write		= my_write,
    .readdir	= my_readdir,
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Not enough arguments." << endl;
        exit(EXIT_SUCCESS);
    }

    string wadPath = argv[argc - 2];

    if (wadPath.at(0) != '/') {
        wadPath = string(get_current_dir_name()) + "/" + wadPath;
    }

    Wad* myWad = Wad::loadWad(wadPath);
    argv[argc -2] = argv[argc - 1];
    argc--;

    return fuse_main(argc, argv, &operations, myWad);
}