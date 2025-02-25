#ifndef MemoryManager_H
#define MemoryManager_H

#include <string>
#include <vector>
#include <iostream>
#include <bitset>
#include <list>
#include <algorithm>
#include <functional>
#include <stdlib.h>

using namespace std;

int bestFit(int sizeInWords, void* list);
int worstFit(int sizeInWords, void* list);
void deleteArray(void* arr);

class MemoryManager{

public:
    MemoryManager(unsigned wordSize, function<int(int, void*)> allocator);
    ~MemoryManager();
    void initialize(size_t sizeInWords);
    void shutdown();
    void* allocate(size_t sizeInBytes);
    void free(void* address);
    void setAllocator(function<int(int, void*)> allocator);
    int dumpMemoryMap(char* filename);
    void* getList();
    void* getBitmap();
    unsigned getWordSize();
    void* getMemoryStart();
    unsigned getMemoryLimit();

private:
    function<int(int, void*)> allocator;
    vector<void*> locationsMemBlocks;
    vector<int> offsetOfMems;
    vector<size_t> bitMap;
    vector<size_t> sizeOfMemBlocks;  // size_t is a type that represents the size of objects in memory
    unsigned wordSize;
    size_t totalSpace;
    size_t remain_bytes;
    char* memoryAddress;
};

#endif