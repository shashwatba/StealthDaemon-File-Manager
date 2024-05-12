#include "NaryTree.h"
#include <regex>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>



class Wad{
  private:
    unsigned int descriptors = 0;
    unsigned int migos = 0;

    std::fstream fileStream;
    Wad(const std::string &path);
    NaryTree* tree;
    char fileMagic[5] = {0};

  public:
    ~Wad();
    static Wad* loadWad(const std::string &path);
    std::string getMagic();
    bool isContent(const std::string &path);
    bool isDirectory(const std::string &path);
    int getSize(const std::string &path);
    int getContents(const std::string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const std::string &path, std::vector<std::string> *directory);
    void createDirectory(const std::string &path);
    void createFile(const std::string &path);
    int writeToFile(const std::string &path, const char *buffer, int length, int offset = 0);

    void navigate(Node* node, Node* newNode, std::vector<std::string> path);
    void managePosition(Node* node, Node* newNode, int length);

};