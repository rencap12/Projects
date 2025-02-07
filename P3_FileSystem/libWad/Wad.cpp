// RENECA CAPUNP P3

#include "Wad.h"


Wad::Wad(const string& path) {
    rootNode = new Node;
    rootNode->name = '/';
    rootNode->isDirectory = true;
    vector<Node*> kids_for_childrenVec;
    rootNode->children = kids_for_childrenVec;
    ifstream file(path);
    inputFile = &file;

    if(file) {
        this->pathName = path;
        file.seekg(0, file.end);
        int length = file.tellg();
        file.seekg(0,file.beg);

        char* magicInfoBuffer = new char[4];
        char* numDescriptorsBuffer = new char[4];
        char* descriptorOffsetBuffer = new char[4];

        file.read(magicInfoBuffer, 4);
        file.read(numDescriptorsBuffer, 4);
        file.read(descriptorOffsetBuffer, 4);

        int numDescriptors = *((int*) numDescriptorsBuffer);
        int descOffset = *((int*) descriptorOffsetBuffer);

        this->magic = magicInfoBuffer;
        this->descriptorNum = numDescriptors;
        this->descriptorOffset = descOffset;

        char* nodeOffsetBuffer = new char[4];
        char* nodeLengthBuffer = new char[4];
        char* nodeNameBuffer = new char[8];

        file.seekg(descOffset); // seek forward descriptor bytes

        for (int i = 0; i < this->descriptorNum; i++) {
            Node* node = new Node;

            file.read(nodeOffsetBuffer, 4);
            file.read(nodeLengthBuffer, 4);
            file.read(nodeNameBuffer, 8);

            uint32_t nOffset = *((uint32_t*) nodeOffsetBuffer);
            uint32_t nLength = *((uint32_t*) nodeLengthBuffer);

            string nName = string(nodeNameBuffer);
            this->descNamesVec.push_back(nName);

            node->offset = nOffset;
            node->length = nLength;
            node->name = nName;
            node->descriptorLocation = this->descriptorOffset + (16 * i);

            vector<int> offsetAndLength;
            offsetAndLength.push_back(nOffset);
            offsetAndLength.push_back(nLength);
            this->descOffsetLengthPairs.push_back(offsetAndLength);

            this->listOfDescriptors.push_back(node);
        }  

        delete[] magicInfoBuffer; 
        delete[] descriptorOffsetBuffer;
        delete[] numDescriptorsBuffer; 
        delete[] nodeNameBuffer;  
        delete[] nodeOffsetBuffer; 
        delete[] nodeLengthBuffer; 

        // making the directory tree

        Node* current = this->rootNode;

        for (int i = 0; i < this->listOfDescriptors.size(); i++) {
            Node* newNode = this->listOfDescriptors[i];
            string str = this-> listOfDescriptors[i]->name;

            if (str.size() > 6 && str.substr(str.length() - 6) == "_START") {
                str.resize(str.size() - 6);
                newNode->name = str;
                newNode->isStart = true;
                newNode->isDirectory = true;
                newNode-> parent = current; // root
                current->children.push_back(newNode); // node gets added to child vec of root
                current = newNode;
            }
            else if (str[0] == 'E' && isdigit(str[1]) && str[2] == 'M' && isdigit(str[3]) && str.size() == 4) {
                newNode->isMapMarker = true;
                newNode->isDirectory = true;
                newNode->parent = current;
                current->children.push_back(newNode);
                current = newNode;

                for (int j = i + 1; j < i + 11; j++) {
                    Node* mapElement = new Node;
                    mapElement->isDirectory = false;
                    mapElement->isContent = true;
                    mapElement->name = this->descNamesVec[j];
                    mapElement->offset = this->descOffsetLengthPairs[j][0];
                    mapElement->length = this->descOffsetLengthPairs[j][1];

                    mapElement->parent = current;
                    current->children.push_back(mapElement);
                }

                i += 10;
                current = current->parent; // move out of that directory
            }
            else if (newNode->name.size() > 4 && newNode->name.substr(newNode->name.length() - 4) == "_END") {
                current = current->parent;
            }
            else { // normal content file   
                newNode->parent = current;
                current->children.push_back(newNode);
                newNode->isContent = true;
            }
        }
    }
    file.close();
}

Wad* Wad::loadWad(const string& path) {
    Wad* wad = new Wad(path);
    return wad;
}

string Wad::getMagic() {
    return this->magic;
}

bool Wad::isContent(const string& path) {
    vector<string> fileNames;
    fileNames = getDirectoryNames(path);
    Node* node = getNode(fileNames, this->rootNode, path);

    if(node){
        return node->isContent;
    }

    return false;
}

bool Wad::isDirectory(const string& path) {

    vector<string> fileNames;
    fileNames = getDirectoryNames(path);

    Node* node = getNode(fileNames, this->rootNode, path);

    if(node){
        return node->isDirectory;
    }

    return false;
}

int Wad::getSize(const string& path) {
    if (isContent(path)) {
        vector<string> fileNames;
        fileNames = getDirectoryNames(path);
        Node* node = getNode(fileNames, this->rootNode, path);
        return node->length;
    }
    return -1;
}

int Wad::getContents(const string& path, char* buffer, int length, int offset) {
    if (isContent(path)) {
        ifstream file(this->pathName);
        file.seekg(0,file.beg);
        vector<string> fileNames;
        fileNames = getDirectoryNames(path);
        Node* node = getNode(fileNames, this->rootNode, path);

        int nodeDataLength = node->length;
        int nodeDataOffset = node->offset;

        if (offset > nodeDataLength) {
            return 0;
        }

        file.seekg(nodeDataOffset + offset);

        if (nodeDataLength <= (offset + length)) {
            int bytesRead = nodeDataLength - offset;
            char* bufferReturn = new char[bytesRead];
            file.read(bufferReturn, bytesRead);
            memcpy(buffer, bufferReturn, bytesRead);
            delete[] bufferReturn;
            return bytesRead;
        }
        else {
            char* bufferReturn2 = new char[length];
            file.read(bufferReturn2, length);
            memcpy(buffer, bufferReturn2, length);
            delete[] bufferReturn2;
            return length;
        }
    }
    else {
        return -1;
    }
}

int Wad::getDirectory(const string& path, vector<string>* directory) {
    if (isDirectory(path)) {
        vector<string> fileNames = getDirectoryNames(path);
        Node* currNode = getNode(fileNames, this->rootNode, path);
        if (currNode) {
            for (int i = 0; i < currNode->children.size(); i++) {
                directory->push_back(currNode->children[i]->name);
            }
            return currNode->children.size();
        }
        return -1; // empty
    }
    return -1; // not a directory
}

void Wad::createDirectory(const string& path) {

    char separator = '/';
    int count = 0;
    string pathNoSlash = path;
    if (path.at(path.size() - 1) == separator) {
        pathNoSlash = pathNoSlash.substr(0, path.size() - 1);
    }

    size_t found = path.find_last_of("/");
    string parentPath = path.substr(0, found);
    string newDirectory = path.substr(found + 1, path.size() - 1);
    size_t found2 = parentPath.find_last_of("/");
    string lastInParent = parentPath.substr(found2 + 1, found - 1);

    if (newDirectory.length() > 2 || lastInParent.length() > 2) {
        return;
    }

    ifstream file(this->pathName);
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    char* readBuffer = new char[length + 32];
    file.read(readBuffer, length);

    for (int i = 0; i < pathNoSlash.size(); i++) {
        if (pathNoSlash.at(i) == separator) {
            count++;
        }
    }

    if (count == 1) { // make directory at root
        Node* rootNodeStart = new Node;
        Node* rootNodeEnd = new Node;
        rootNodeStart->isDirectory = true;
        rootNodeEnd->isDirectory = true;
        rootNodeStart->name = lastInParent;
        rootNodeEnd->name = lastInParent + "_END";
        int lastNodeOffset = this->listOfDescriptors.back()->offset + this->listOfDescriptors.back()->length;

        if (this->listOfDescriptors.back()->length == 0) { // last in list is a directory
            lastNodeOffset += 16;
        }

        //update tree and list descriptors
        rootNodeStart->offset = lastNodeOffset;
        rootNodeEnd->offset = lastNodeOffset + 16;
        this->listOfDescriptors.push_back(rootNodeStart);
        this->listOfDescriptors.push_back(rootNodeEnd);
        this->rootNode->children.push_back(rootNodeStart);

        ofstream file(this->pathName);

        file.write(readBuffer, 4); // write magic

        char* numDescriptorsBuffer = new char[4]; // write # descriptors
        int numDescriptors = this->descriptorNum + 2;
        memcpy(numDescriptorsBuffer, &numDescriptors, 4);
        file.write(numDescriptorsBuffer, 4);

        file.write(readBuffer + 8, length - 8); // write descriptors in rest of file
        char* nodeOffsetBuffer = new char[4];
        char* nodeLengthBuffer = new char[4];
        char* nodeNameBuffer = new char[8];

        uint32_t* nOffset = (uint32_t*) (nodeOffsetBuffer);
        uint32_t* nLength = (uint32_t*) (nodeLengthBuffer);

        nOffset = (uint32_t*)(&rootNodeStart->offset); //start node
        nLength = (uint32_t*)(&rootNodeStart->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);

        string name = rootNodeStart->name + "_START"; 
        file.write(name.c_str(), 8);

        nOffset = (uint32_t*)(&rootNodeEnd->offset); //end node
        nLength = (uint32_t*)(&rootNodeEnd->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);
        file.write(rootNodeEnd->name.c_str(), 8);

        delete[] numDescriptorsBuffer;
        delete[] nodeOffsetBuffer;
        delete[] nodeLengthBuffer;
        delete[] nodeNameBuffer;

        file.close();
    }
    else if (isDirectory(parentPath)) {
        int index = 0;
        int descriptorOffset = 0;
        bool reachEnd = false;
        int nodeLumpOffset = 0;

        // find last node
        for (int i = 0; i < this->listOfDescriptors.size(); i++) {
            string lastInParentEnd = lastInParent + "_END";
            if (reachEnd == true) {
                listOfDescriptors[i]->descriptorLocation += 32;
            }
            // END
            if (listOfDescriptors[i]->name == lastInParentEnd) {
                reachEnd = true;
                index = i;
                descriptorOffset = listOfDescriptors[i]->descriptorLocation;
                listOfDescriptors[i]->descriptorLocation = descriptorOffset + 32;
                nodeLumpOffset = listOfDescriptors[i]->offset;
            }
        }

        // create new nodes and set vars
        Node* nodeStart = new Node;
        Node* nodeEnd = new Node;
        nodeStart->name = newDirectory;
        nodeStart->offset = nodeLumpOffset;
        nodeStart->isDirectory = true;
        nodeEnd->name = newDirectory + "_END";
        nodeEnd->offset = nodeLumpOffset + 16;
        nodeEnd->isDirectory = true;

        vector<string> fileNames = getDirectoryNames(parentPath);
        Node* currNode = getNode(fileNames, this->rootNode, path);
        currNode->children.push_back(nodeStart);
        auto it = listOfDescriptors.emplace(listOfDescriptors.begin() + index, nodeEnd);
        this->listOfDescriptors.emplace(it, nodeStart);

        file.seekg(0, file.beg);
        char* buffer = new char[length - descriptorOffset];
        file.seekg(descriptorOffset);
        file.read(buffer, length - descriptorOffset);

        ofstream file(this->pathName);

        //write magic
        file.write(readBuffer, 4);

        //write # of descriptors
        char* numDescriptorsBuffer = new char[4];
        int numDescriptors = this->descriptorNum + 2;
        memcpy(numDescriptorsBuffer, &numDescriptors, 4);
        file.write(numDescriptorsBuffer, 4);

        file.write(readBuffer + 8, descriptorOffset - 8); // write all before offset

        char* nodeOffsetBuffer = new char[4]; // write new nodes
        char* nodeLengthBuffer = new char[4];
        char* nodeNameBuffer = new char[8];

        uint32_t* nOffset = (uint32_t*)(nodeOffsetBuffer);
        uint32_t* nLength = (uint32_t*)(nodeLengthBuffer);

        nOffset = (uint32_t*)(&nodeStart->offset);
        nLength = (uint32_t*)(&nodeStart->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);

        string name = nodeStart->name + "_START";
        file.write(name.c_str(), 8);

        nOffset = (uint32_t*)(&nodeEnd->offset);
        nLength = (uint32_t*)(&nodeEnd->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);
        file.write(nodeEnd->name.c_str(), 8);

        // write rest of file
        file.write(buffer, length - descriptorOffset);

        delete[] buffer;
        delete[] numDescriptorsBuffer;
        delete[] nodeOffsetBuffer;
        delete[] nodeLengthBuffer;
        delete[] nodeNameBuffer;
        delete[] readBuffer;

        file.close();
    }
}

void Wad::createFile(const string& path) {
    size_t found = path.find_last_of("/");
    string parentPath = path.substr(0, found);
    string newFile = path.substr(found + 1, path.size() - 1);
    size_t found2 = parentPath.find_last_of("/");
    string lastInParent = parentPath.substr(found2 + 1, found - 1);
    char separator = '/';
    int count = 0;

    if (newFile.length() > 8 || lastInParent.length() > 8) { // name too long
        return;
    }
    if (newFile[0] == 'E' && isdigit(newFile[1]) && newFile[2] == 'M' && isdigit(newFile[3]) && newFile.size() == 4) {
        return;
    }
    else if (lastInParent[0] == 'E' && isdigit(lastInParent[1]) && lastInParent[2] == 'M' && isdigit(lastInParent[3]) && lastInParent.size() == 4) {
        return;
    }
    else if (newFile.substr(newFile.length() - 6) == "_START") {
        return;
    }
    else if (newFile.substr(newFile.length() - 4) == "_END") {
        return;
    }

    // read in while wad file
    ifstream file(this->pathName);
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    char* readBuffer = new char[length + 16];
    file.read(readBuffer, length);

    string pathNoSlash = path;
    if (path.at(path.size() - 1) == separator) {
        pathNoSlash = pathNoSlash.substr(0, path.size() - 1);
    }

    for (int i = 0; i < pathNoSlash.size(); i++) {
        if (pathNoSlash.at(i) == separator) {
            count++;
        }
    }

    if (count == 1) {
        // create new nodes and set vars
        Node* rootNodeFile = new Node;

        rootNodeFile->isDirectory = false;
        rootNodeFile->isContent = true;
        rootNodeFile->name = newFile;

        // update tree
        rootNodeFile->offset = 0;
        rootNodeFile->length = 0;
        rootNodeFile->descriptorLocation = this->listOfDescriptors.back()->descriptorLocation + 16;
        this->listOfDescriptors.push_back(rootNodeFile);
        this->rootNode->children.push_back(rootNodeFile);

        ofstream file(this->pathName);

        // write magic
        file.write(readBuffer, 4);

        // write # of descriptors
        char* numDescriptorsBuffer = new char[4];
        int numDescriptors = this->descriptorNum + 1;
        memcpy(numDescriptorsBuffer, &numDescriptors, 4);
        file.write(numDescriptorsBuffer, 4);

        file.write(readBuffer + 8, length - 8); // write descriptors in the rest
        char* nodeOffsetBuffer = new char[4]; // write new nodes
        char* nodeLengthBuffer = new char[4];
        char* nodeNameBuffer = new char[8];

        uint32_t* nOffset = (uint32_t*)(nodeOffsetBuffer);
        uint32_t* nLength = (uint32_t*)(nodeLengthBuffer);

        nOffset = (uint32_t*)(&rootNodeFile->offset);
        nLength = (uint32_t*)(&rootNodeFile->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);

        string name = rootNodeFile->name;
        file.write(name.c_str(), 8);

        delete[] numDescriptorsBuffer;
        delete[] nodeOffsetBuffer;
        delete[] nodeLengthBuffer;
        delete[] nodeNameBuffer;

        file.close();
    }
    else if (isDirectory(parentPath)) { // create file in directory
        int index = 0;
        int descriptorOffset = 0;
        bool reachEnd = false;
        int nodeLumpOffset = 0;

        // find last node
        for (int i = 0; i < this->listOfDescriptors.size(); i++) {
            string lastInParentEnd = lastInParent + "_END";
            if (reachEnd == true) {
                listOfDescriptors[i]->descriptorLocation += 16;
            }
            // END
            if (listOfDescriptors[i]->name == lastInParentEnd) {
                reachEnd = true;
                index = i;
                descriptorOffset = listOfDescriptors[i]->descriptorLocation;
                listOfDescriptors[i]->descriptorLocation = descriptorOffset + 16;
            }
        }

        // create new nodes and set vars
        Node* fileNode = new Node;
        fileNode->name = newFile;
        fileNode->offset = 0;
        fileNode->length = 0;
        fileNode->descriptorLocation = descriptorOffset;
        fileNode->isDirectory = false;
        fileNode->isContent = true;

        vector<string> fileNames = getDirectoryNames(parentPath);
        Node* currNode = getNode(fileNames, this->rootNode, path);
        currNode->children.push_back(fileNode);
        this->listOfDescriptors.emplace(listOfDescriptors.begin() + index, fileNode);

        file.seekg(0, file.beg);
        char* buffer = new char[length - descriptorOffset];
        file.seekg(descriptorOffset);
        file.read(buffer, length - descriptorOffset);

        ofstream file(this->pathName);

        file.write(readBuffer, 4); // write magic

        // write # of descriptors
        char* numDescriptorsBuffer = new char[4];
        int numDescriptors = this->descriptorNum + 1;
        memcpy(numDescriptorsBuffer, &numDescriptors, 4);
        file.write(numDescriptorsBuffer, 4);

        file.write(readBuffer + 8, descriptorOffset - 8); // write all before offset

        char* nodeOffsetBuffer = new char[4]; // write new nodes
        char* nodeLengthBuffer = new char[4];
        char* nodeNameBuffer = new char[8];

        uint32_t* nOffset = (uint32_t*)(nodeOffsetBuffer);
        uint32_t* nLength = (uint32_t*)(nodeLengthBuffer);

        nOffset = (uint32_t*)(&fileNode->offset);
        nLength = (uint32_t*)(&fileNode->length);

        file.write((char*) nOffset, 4);
        file.write((char*) nLength, 4);

        string name = fileNode->name;
        file.write(name.c_str(), 8);

        // write rest of file
        file.write(buffer, length - descriptorOffset);

        delete[] buffer;
        delete[] numDescriptorsBuffer;
        delete[] nodeOffsetBuffer;
        delete[] nodeLengthBuffer;
        delete[] nodeNameBuffer;
        delete[] readBuffer;

        file.close();
    }
}

int Wad::writeToFile(const string& path, const char* buffer, int length, int offset) {
    if(isContent(path)) {
        vector<string> fileNames;
        fileNames = getDirectoryNames(path);
        Node* node = getNode(fileNames, this->rootNode, path);

        // read in whole wad file
        ifstream file(this->pathName);
        file.seekg(0, file.end);
        int wholeFileLength = file.tellg();
        file.seekg(0, file.beg);
        char* readBuffer = new char[wholeFileLength];
        file.read(readBuffer, wholeFileLength);

        int nodeDataLength = node->length;
        if (nodeDataLength != 0) {
            return 0;
        }

        if (isDirectory(path) == true) {
            return -1;
        }
        
        char* buf = new char[length];
        memcpy(buf, buffer + offset, length);

        file.close();

        ofstream OUTfile(this->pathName);

        // write magic
        OUTfile.write(readBuffer, 4);
        // write num descrip
        OUTfile.write(readBuffer + 4, 4);

        //write descriptor offset
        char* descOffset = new char[4];
        int descriptorOffset = this->descriptorOffset + length;
        memcpy(descOffset, &descriptorOffset, 4);
        OUTfile.write(descOffset, 4);

        OUTfile.write(readBuffer + 12, this->descriptorOffset - 12); // write lumpdata

        OUTfile.write(buf, length); // write new lumpdata

        OUTfile.write(readBuffer + this->descriptorOffset, node->descriptorLocation - this->descriptorOffset);

        node->length = length; // update node length
        node->offset = this->descriptorOffset;

        char* nodeOffsetBuffer = new char[4]; // write new nodes
        char* nodeLengthBuffer = new char[4];

        uint32_t* nOffset = (uint32_t*)(nodeOffsetBuffer);
        uint32_t* nLength = (uint32_t*)(nodeLengthBuffer);

        nOffset = (uint32_t*)(&node->offset);
        nLength = (uint32_t*)(&node->length);

        OUTfile.write((char*) nOffset, 4);
        OUTfile.write((char*) nLength, 4);

        OUTfile.write(readBuffer + node->descriptorLocation + 8, wholeFileLength - node->descriptorLocation - 8);

        this->descriptorOffset = descriptorOffset;

        delete[] buf;
        delete[] descOffset;
        delete[] nodeOffsetBuffer;
        delete[] nodeLengthBuffer;
        delete[] readBuffer;

        OUTfile.close();
        

        return length;
    }

    return -1; // path has no content to write
}

// get current node at the path
Node* Wad::getNode(vector<string> files, Node* curr, const string& path) {
    if (files.empty() && path == "") {
        curr->isDirectory = false;
        return curr;
    }
    else if (files.empty() && path == "/") {
        curr->isDirectory = true;
        return curr;
    }
    else if (files.size() == 1) {
        for (Node* node : curr->children) {
            if (files[0] == node-> name) {
                return node;
            }
        }
    }
    else {
        for (Node* n : curr->children) {
            if (files[0] == n->name) {
                curr = n;
                files.erase(files.begin());
                return getNode(files, curr, path);
            }
        }
    }
    return nullptr;
}

vector<string> Wad::getDirectoryNames(const string& path) {
    string pathName = path;
    string separator = "/";
    vector<string> files;

   
    if (pathName == "/") {
        return files;
    }
    
    if (pathName == "") {
        return files;
    }

    string subString;
    size_t position = 0;
    pathName = pathName.substr(1, pathName.length() - 1);

    while (position != string::npos) {
        position = pathName.find(separator);
        if (position != string::npos) {
            subString = pathName.substr(0, position);
            files.push_back(subString);
            pathName.erase(0, position + separator.length());
        }
        else if (position == string::npos && pathName.length() > 0) {
            files.push_back(pathName);
        }
    }

    return files;
}


