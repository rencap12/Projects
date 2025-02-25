#include <climits>
#include <fcntl.h>
#include <sys/stat.h>
#include "MemoryManager.h"

using namespace std;

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator) { 
    this->wordSize = wordSize; // to private var
    this->allocator = allocator; // to private var
    this->memoryAddress = nullptr; // initial ptr to current mem arr
}

MemoryManager::~MemoryManager() { // indirect call
    shutdown();
    delete[] memoryAddress;
}

void MemoryManager::initialize(size_t sizeInWords) { // check size of req size, cleans prev block

    if (sizeInWords < +0 || sizeInWords > 65536) {
       // cout << "requested memory is not appropriate" << endl;
        return;
    }

    totalSpace = (wordSize * sizeInWords); // instantiates contiguous array based on this amount of bytes
    remain_bytes = totalSpace;
    bitMap = vector<size_t>(sizeInWords);

    if (memoryAddress) { // prev block , clear if this func is already has this
        delete[] memoryAddress;
    }

    memoryAddress = new char[totalSpace]; // new instant block from request that is appropriate, reinit too 
}

void MemoryManager::shutdown() { // clears all heap memory based on private vectors
    locationsMemBlocks.clear();
    offsetOfMems.clear();
    bitMap.clear();
    sizeOfMemBlocks.clear();
}


void* MemoryManager::allocate(size_t sizeInBytes) { // return ptr to starting address of new alloc space
    unsigned bsize = sizeInBytes;

    if (sizeInBytes <= 0) {
      //  cout << "too small of given bytes" << endl;
        return nullptr;
    }
    if (sizeInBytes > remain_bytes) {
     //   cout << "too many given bytes" << endl;
        return nullptr;
    }

    unsigned words = bsize / wordSize; 
    uint16_t* list = (uint16_t*)getList(); // call getList() to find where to put data
    int offsetInt = allocator(words, (void*)list);
    deleteArray(list);

    if (offsetInt < 0) {
      //  cout << "no memory available" << endl;
        return nullptr;
    }

    int wordSpace = words + offsetInt;
    int offsetBytes = offsetInt * wordSize;
    remain_bytes -= sizeInBytes;
    char* currentMemoryAddress = memoryAddress;
    currentMemoryAddress = currentMemoryAddress + (uint16_t)offsetBytes; // new location
    locationsMemBlocks.push_back(currentMemoryAddress);
    sizeOfMemBlocks.push_back(sizeInBytes);
    offsetOfMems.push_back(offsetInt); 

    for (int i = offsetInt; i < wordSpace; i++) {
        bitMap.at(i) = 1;
    }

    return currentMemoryAddress;
}

void MemoryManager::free(void* address) { // account for new holes and/or existing ones
    int intOffset = 0;
    size_t removeMemory;

    for (int i = 0; i < locationsMemBlocks.size(); i++) {
        if (locationsMemBlocks.at(i) == address) {
            intOffset = offsetOfMems.at(i);
            removeMemory = sizeOfMemBlocks.at(i);
            locationsMemBlocks.erase(locationsMemBlocks.begin() + i);
            offsetOfMems.erase(offsetOfMems.begin() + i);
            sizeOfMemBlocks.erase(sizeOfMemBlocks.begin() + i);
        }
    }

    remain_bytes += removeMemory; // add free space to available bytes
    size_t removeSpace = ((removeMemory / wordSize) + intOffset);

    for (size_t i = intOffset; i < removeSpace; i++) {
        bitMap.at(i) = 0;
    }
}

void MemoryManager::setAllocator(function<int(int, void*)> allocator) { // change priv var
    this->allocator = allocator;
}

int MemoryManager::dumpMemoryMap(char* filename) { // use POSIX (Portable Operating System Interface) | prints current list of holes
    uint16_t* list = (uint16_t*)getList();

    if (list == nullptr) {
        deleteArray(list);
        return -1;
    }

    string holeList;
    uint16_t listSize = (uint16_t)((list[0] * 2) + 1);

    for (int i = 1; i < listSize; i += 2) { // based on format in pdf
        holeList.append("[" + to_string(list[i]) + ", " + to_string(list[i + 1]) + "]");
        if ((i + 2) < listSize) {
            holeList.append(" - ");
        }
    }

    FILE* fd;
    fd = fopen(filename, "w");
    fwrite(holeList.c_str(), (size_t)holeList.length(), 1, fd);
    fclose(fd);
    deleteArray(list);
    return 0;
}

void* MemoryManager::getList() { // returns pointer to 2-byte array that has the holes

    if (locationsMemBlocks.size() == 0) {
        return nullptr;
    }

    vector<int> holeSpaces;
    vector<int> holeLocations;
    int holeSpace = 0;

    for (int i = 0; i < bitMap.size(); i++) { 
        if (bitMap.at(i) == 0) {
            holeSpace += 1;
            
            if (bitMap.size() != (i + 1)) {
                continue;
            }
            else {
                holeSpaces.push_back(holeSpace);
                holeLocations.push_back(i - (holeSpace - 1));
                break;
            }
        }
        else if (bitMap.at(i) == 1) {
            if (holeSpace != 0) {
                holeSpaces.push_back(holeSpace);
                holeLocations.push_back(i - holeSpace);
                holeSpace = 0;
            }
        }
    }

    uint16_t* list = new uint16_t[((holeSpaces.size() * 2) + 1)]; // to ret
    list[0] = (uint16_t)holeSpaces.size(); // number of holes
    int holeIndex = 0;
    uint16_t listSize = (uint16_t)((list[0] * 2) + 1);

    for (uint16_t i = 1; i < listSize; i += 2) {
        list[i] = (uint16_t)holeLocations.at(holeIndex); // offset
        list[i + 1] = (uint16_t)holeSpaces.at(holeIndex); // length
        holeIndex++;
    }
    holeLocations.clear();
    holeSpaces.clear();
    return list;
}  

void* MemoryManager::getBitmap() { // little endian is  handled, return ptr to array of 1 byte type to rep holes + blocks
     int mapSize = 0;

     if (bitMap.size() == 0) {
      //  cout << "empty bitmap" << endl;
        return nullptr;
     }

     if (bitMap.size() % 8 == 0) {
        mapSize = bitMap.size() / 8;
     }
     else {
        mapSize = ((bitMap.size() / 8) + 1);
     }

     uint8_t* map = new uint8_t[mapSize + 3];
     
     if (mapSize / 16 > 15) { // 2 bytes eachy
        map[0] = 255; // 1st byte -> 255 = 1
        map[1] = mapSize % 255;
     }
     else {
        map[0] = mapSize % 255;
        map[1] = 0;
     }

     uint8_t placeMult = 1;
     uint8_t byteTotal = 0;
     int mapPos = 2;

     for (uint8_t i = 0; i < bitMap.size(); i++) {
        if (placeMult == 128) {
            byteTotal = (placeMult * bitMap.at(i)) + byteTotal;
            map[mapPos] = byteTotal;
            placeMult = 1;
            byteTotal = 0;
            mapPos++;
        }
        else {
            byteTotal = (placeMult * bitMap.at(i)) + byteTotal;
            placeMult *= 2;
        }

        if ((i + 1) == bitMap.size()) {
            map[mapPos] = byteTotal;
            break;
        }
     }

     return map;
}

unsigned MemoryManager::getWordSize() {
    return wordSize;
}

void* MemoryManager::getMemoryStart() {
    return memoryAddress;
}

unsigned MemoryManager::getMemoryLimit() {
    return totalSpace;
}

int bestFit(int sizeInWords, void* list) { // returns the offset of the smallest possible hole that still fits sizeInWords
    uint16_t* fitList = (uint16_t*)list;

    if (fitList == nullptr) {
        return 0;
    }

    int smallestSize = INT_MAX; // guaranteed to fit the data
    int location = 0;
    uint16_t listSize = (uint16_t)((fitList[0] * 2) + 1);

    for (int i = 1; i < listSize; i += 2) {
        if (fitList[i + 1] == (uint16_t)sizeInWords) {
            int spaceFits = fitList[i];
            return spaceFits;
        }
        else if (fitList[i + 1] > (uint16_t)sizeInWords) {
            if (smallestSize > fitList[i + 1]) {
                smallestSize = fitList[i + 1];
                location = fitList[i];
            }
        }
    }

    return location;
}

int worstFit(int sizeInWords, void* list) {
    uint16_t* fitList = (uint16_t*)list;
    
    if (fitList == nullptr) {
        return 0;
    }

    int biggestSize = 0;
    int location = 0;
    uint16_t listSize = (uint16_t)((fitList[0] * 2) + 1);

    for (int i = 1; i < listSize; i += 2) {
        if (fitList[i + 1] >= (uint16_t)sizeInWords) {
            if (biggestSize <= fitList[i + 1]) {
                biggestSize = fitList[i + 1];
                location = fitList[i];
            }
        }
    }
    
    return location;
}

void deleteArray(void* arr) {
    uint16_t* deleteArr = (uint16_t*)arr;
    if (deleteArr) {
        delete[] deleteArr;
        deleteArr = nullptr;
    }
}