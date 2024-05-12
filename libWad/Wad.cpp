#include "Wad.h"
#include <iostream>

Wad::~Wad(){
  fileStream.close(); // Close the file stream when the Wad object is destroyed
  delete tree; // Free memory allocated for the tree
}

Wad* Wad::loadWad(const std::string &path){
  Wad* temp = new Wad(path); // Create a new Wad object with the specified file path
  return temp;
}

std::string Wad::getMagic(){
  return fileMagic; // Return the magic number of the WAD file
}

void Wad::createDirectory(const std::string &path){
  std::vector<std::string> parsedString = spotFinder(path); // Parse the path to find directory names
  if(parsedString.back().size() > 2){
    return; // Exit if the last directory name is longer than 2 characters
  }
  std::string name = parsedString.back();
  if(name.size() > 2){
    return; // Check again and return if name is longer than 2 characters, may be redundant
  }
  parsedString.pop_back(); // Remove the last element, which is now stored in 'name'

  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return; // Return if no node is found for the directory path
  }
  char arr[name.length() + 1];
  Node* newNode = new Node(0, 0, strcpy(arr, name.c_str()) , "namespace"); // Create a new directory node
  tree->insert(node, newNode); // Insert the new node in the tree at the correct location
  navigate(node, newNode, parsedString); // Adjust file pointers in the WAD file

  descriptors += 2; // Update descriptor count
  std::streampos currentPosition = fileStream.tellg();

  unsigned int size = 0;
  fileStream.seekg(4);
  fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
  fileStream.seekp(4);
  fileStream.write(reinterpret_cast<char*>(&descriptors), sizeof(descriptors));
  fileStream.flush();
  fileStream.seekg(4);
  fileStream.read(reinterpret_cast<char*>(&size),sizeof(size));
  fileStream.flush();

  fileStream.seekp(migos);
  fileStream.seekg(migos);

  for(unsigned int i = 0; i < descriptors; i++){
    unsigned int desOffset;
    fileStream.read(reinterpret_cast<char*>(&desOffset), sizeof(desOffset));
    unsigned int size;
    fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    char name[8];
    fileStream.read(name, 8);
    std::string strName(name);
  }
}

void Wad::createFile(const std::string &path){
  // Similar structure to createDirectory, adapted for creating files
  std::vector<std::string> parsedString = spotFinder(path);
  std::regex pattern("E[0-9]M[0-9]");
  std::regex patternStart(".{1,2}_START");
  std::regex patternEnd(".{1,2}_END");
  std::smatch match;
  if(parsedString.back().size() > 8){
   return;
  }
  std::string name = parsedString.back();
  parsedString.pop_back();

  if(std::regex_search(name, match, patternStart) || std::regex_search(name, match, patternEnd) || std::regex_search(name, match, pattern)){ 
    return; // Do not create files with reserved names indicating map data or namespace boundaries
  }

  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return;
  }
  if(node->getType() == "map" || node->getType() == "file"){
    return; // Do not overwrite map or existing file nodes
  }
  char arr[name.length() + 1]; 
  Node* newNode = new Node(0, 0, strcpy(arr, name.c_str()) , "file"); // Create a new file node
  tree->insert(node, newNode);
  navigate(node, newNode, parsedString); // Update file structure in WAD

  descriptors += 1; // Increment descriptor count
  unsigned int size = 0;
  fileStream.seekg(4);
  fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
  fileStream.seekp(4);
  fileStream.write(reinterpret_cast<char*>(&descriptors), sizeof(descriptors));
  fileStream.flush();
  fileStream.seekg(4);
  fileStream.read(reinterpret_cast<char*>(&size),sizeof(size));
  fileStream.flush();
  fileStream.seekp(migos);  
  fileStream.seekg(migos);
}

void Wad::navigate(Node* node, Node* newNode, std::vector<std::string> path){
  // Detailed handling of namespace and file descriptors within the WAD file
  std::vector<std::string> matchString;
  matchString.push_back("/");

  unsigned int zero = 0;
  std::streampos currentPositionG = fileStream.tellg();
  std::streampos currentPositionP = fileStream.tellp();
  fileStream.seekg(migos);
  if(node->getName() == "/"){
    fileStream.seekp(migos + (descriptors * 16));
    currentPositionP = fileStream.tellp();
    if(newNode->getType() == "namespace"){
      std::string tempString = newNode->getName() + "_START";
      std::string tempString2 = newNode->getName() + "_END";
      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(tempString.c_str(), 8);

      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(tempString2.c_str(), 8);
      fileStream.flush();
      return;
    }
   else if(newNode->getType() == "file"){
      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(reinterpret_cast<const char*>(&zero), 4);
      fileStream.write(newNode->getName().c_str(), 8);
      fileStream.flush();
      return;
   }
  return;
  }
 // Node* curDirectory = tree->findFileRootPath();
  for(unsigned int i = 0; i < descriptors; i++){
    // This loop manages directory navigation and file adjustments within the WAD file
    std::regex pattern("E[0-9]M[0-9]");
    std::regex patternStart(".{1,2}_START");
    std::regex patternEnd(".{1,2}_END");
    std::smatch match;
    unsigned int offset;
    fileStream.read(reinterpret_cast<char*>(&offset), sizeof(offset));
    unsigned int size;
    fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    char name[8];
    fileStream.read(name, 8);
    std::string strName(name);
    if(std::regex_search(strName, match, patternStart)){
      matchString.push_back(strName.substr(0, strName.find('_')));
    }
    if(std::regex_search(strName, match, patternEnd)){
      bool areEqual = true;
      if(matchString.size() == path.size()){
        for(size_t i = 0; i < path.size(); i++){
            if(path[i] != matchString[i]){
              areEqual = false;
            }
          }
      }
      else{
        areEqual = false;
      }
      if(strName == node->getName() + "_END" && areEqual){
        // Logic to adjust the file descriptors in response to directory changes
        fileStream.seekg(migos + (i) * 16);
        char buffer[(descriptors - i) * 16];
        fileStream.read(buffer, sizeof(buffer));

        if(newNode->getType() == "namespace"){
	  fileStream.seekp(migos + (i+2) * 16);
          fileStream.write(buffer, sizeof(buffer));
          fileStream.seekp(migos + (i) * 16); 

          std::string tempString = newNode->getName() + "_START";
          std::string tempString2 = newNode->getName() + "_END";
          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(tempString.c_str(), 8);

          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(tempString2.c_str(), 8);
	  fileStream.flush();
          break;
        }
        else if(newNode->getType() == "file"){
	  fileStream.seekp(migos + (i+1) * 16);
          fileStream.write(buffer, sizeof(buffer));
          fileStream.seekp(migos + (i) * 16); 

          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(reinterpret_cast<const char*>(&zero), 4);
          fileStream.write(newNode->getName().c_str(), 8);
          fileStream.flush();
          break;
        }
      }
      matchString.pop_back();
    }
  }
}

Wad::Wad(const std::string &path) {
  fileStream.open(path, std::ios::in | std::ios::out |std::ios::binary);
  char nameArr[1] = {'/'};
  this->tree = new NaryTree(nameArr);
  fileStream.read(this->fileMagic, 4);

  fileStream.read(reinterpret_cast<char*>(&descriptors), sizeof(descriptors));
  fileStream.read(reinterpret_cast<char*>(&migos), sizeof(migos));

  std::regex pattern("E[0-9]M[0-9]");
  std::regex patternStart(".{1,2}_START");
  std::regex patternEnd(".{1,2}_END");
  std::smatch match;
  int count = 0;

  fileStream.seekg(migos);
  Node* curDirectory = tree->findFileRootPath();
  for(unsigned int i = 0; i < descriptors; i++){
    unsigned int offset;
    fileStream.read(reinterpret_cast<char*>(&offset), sizeof(offset));
    unsigned int size;
    fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    char name[8];
    fileStream.read(name, 8);
    std::string strName(name);
    if(std::regex_search(strName, match, patternStart)){
      char arr[2];
      auto charPos = strName.find('_');
      strcpy(arr, strName.substr(0, charPos).c_str());
      Node* node = new Node(offset, size, arr, "namespace");
      tree->insert(curDirectory, node);
      curDirectory = node;
    }
    else if(std::regex_search(strName, match, pattern)){ 
      Node* node = new Node(offset, size, name, "map");
      tree->insert(curDirectory, node);
      curDirectory = node;
      count += 10;
    }
    else if (std::regex_search(strName, match, patternEnd)){
      if(curDirectory->getName() == "/"){
        continue;
      }
      curDirectory = curDirectory->getParent();
      if(curDirectory == nullptr){
	 curDirectory = tree->findFileRootPath();
      }
    }
    else if(count != 0){
      count--;
      Node* node = new Node(offset, size, name);
      tree->insert(curDirectory, node);
      if(count == 0 || count % 10 == 0){
        curDirectory = curDirectory->getParent();
      }
    }
    else{
      Node* node = new Node(offset, size, name);
      tree->insert(curDirectory, node);
    }
  }
}




bool Wad::isContent(const std::string &path){
  std::vector<std::string> parsedString = spotFinder(path);
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return false;
  }
  if(node->getType() != "file"){
    return false;
  }
  return true;
}


bool Wad::isDirectory(const std::string &path){
  std::vector<std::string> parsedString = spotFinder(path);
  if(path.empty()){
    return false;
  }
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return false;
  }
  if(node->getType() == "map" || node->getType() == "namespace"){
    return true;
  }
  return false;
}

int Wad::getSize(const std::string &path){
  std::vector<std::string> parsedString = spotFinder(path);
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return -1;
  }
  if(node->getType() != "file"){
    return -1;
  }
  return node->getLength();
}

int Wad::getContents(const std::string &path, char *buffer, int length, int offset){
  std::vector<std::string> parsedString = spotFinder(path);
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return -1;
  }
  if(node->getType() != "file"){
    return -1;
  }

  if(length + offset > node->getLength()){
    length = node->getLength() - offset;
  }
  fileStream.seekg(node->getOffset() + offset); 
  int bytesRead = fileStream.read(buffer, length).gcount();
  return bytesRead;
}

int Wad::getDirectory(const std::string &path, std::vector<std::string> *directory){
  std::vector<std::string> parsedString = spotFinder(path);
  if(parsedString.empty()){
    return -1;
  }
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return -1;
  }
  int count = 0;
  if(node->getType() == "map" || node->getType() == "namespace"){
    for(Node* child : node->getChildren()){
      directory->push_back(child->getName());
      count++;
    }
  }
  else{
    return -1;
  }
  
  return count;
}

int Wad::writeToFile(const std::string &path, const char *buffer, int length, int offset){
  std::vector<std::string> parsedString = spotFinder(path);
  Node* node = tree->search(tree->findFileRootPath(), parsedString);
  if(node == nullptr){
    return -1;
  }
  if(node->getType() != "file"){
    return -1;
  }
 // Node* tempNode = node;
  if(node->getLength() != 0){
    return 0;
  }
  fileStream.seekg(migos);

  char readBuffer[descriptors * 16];
  fileStream.read(readBuffer, sizeof(readBuffer));
  buffer += offset;
  fileStream.seekp(migos);
  int beforeWrite = fileStream.tellp();

  int bytesWritten = fileStream.write(buffer, length).tellp();
  bytesWritten -= beforeWrite;
  int newOffset = migos + bytesWritten;
  fileStream.flush();
  fileStream.write(readBuffer, sizeof(readBuffer));
  fileStream.flush();
  fileStream.seekp(8);
  fileStream.write(reinterpret_cast<char*>(&newOffset), sizeof(newOffset));
  fileStream.flush();
  fileStream.seekp(newOffset);
  fileStream.seekg(newOffset);
  for(unsigned int i = 0; i < descriptors; i++){
    unsigned int desOffset;
    fileStream.read(reinterpret_cast<char*>(&desOffset), sizeof(desOffset));
    unsigned int size;
    fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    char name[8];
    fileStream.read(name, 8);
    std::string strName(name);
    if(strName == node->getName() && size == 0){
      fileStream.seekp(newOffset + i * 16);
      fileStream.write(reinterpret_cast<char*>(&migos), sizeof(newOffset));
      fileStream.write(reinterpret_cast<char*>(&bytesWritten), sizeof(bytesWritten));
      fileStream.flush();
      break;
    }
  }
  node->setOffset(migos);
  node->setLength(bytesWritten);

  migos = newOffset;

  return bytesWritten;
}