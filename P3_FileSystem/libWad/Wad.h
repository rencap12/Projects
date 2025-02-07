#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stack>
#include <cstring>
#include <string>
#include <cctype>
#include <cstdint>


using namespace std;

struct Node {

    public:
        string name;
        int length = 0;
        int offset = 0;
        int descriptorLocation = 0;
        bool isEnd = false;
        bool isDirectory = false;
        bool isMapMarker = false;
        bool isStart = false;
        bool isContent = false;
        char* data = nullptr;
        vector<Node*> children;
        Node* parent = nullptr;
};

class Wad {

    private:
        Wad(const string& path);

    public:
        Node* rootNode;
        string magic = "";
        int descriptorNum = 0;
        int descriptorOffset = 0;
        vector<Node*> listOfDescriptors;
        string pathName = "";
        string currentPath = "";
        ifstream* inputFile;
        vector<string> descNamesVec;
        vector<vector<int>> descOffsetLengthPairs; // pairs for offset and length

        static Wad* loadWad(const string& path);
        string getMagic();
        bool isContent(const string& path);
        bool isDirectory(const string& path);
        int getSize(const string& path);
        int getContents(const string& path, char* buffer, int length, int offset = 0);
        int getDirectory(const string& path, vector<string>* directory);
        void createDirectory(const string& path);
        void createFile(const string& path);
        int writeToFile(const string& path, const char* buffer, int length, int offset = 0);

        Node* getNode(vector<string> files, Node* curr, const string& path);
        vector<string> getDirectoryNames(const string& path);
};
