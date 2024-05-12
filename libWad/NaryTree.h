#include <string>
#include <cstring>
#include <vector>
#include <iostream>

class Node{
  private:
    int offset = 0;
    int length = 0;
    char name[8];
    int data = 0;
    Node* parent;

    std::vector<Node*> children;
    std::string type;
  public:
    Node(int data){
      this->data = data;
    }
    Node(int offset, int length, char data[], std::string type = "file"){

      this->length = length;
      this->offset = offset;
      strncpy(this->name, data, sizeof(name));
      this->type = type;
    }
    friend class NaryTree;
    Node* getParent(){
      return this->parent;
    }
    std::string getName(){
      return this->name;
    }
    std::string getType(){
      return this->type;
    }
    int getLength(){
      return this->length;
    }
    int getOffset(){
      return this->offset;
    }
    std::vector<Node*> getChildren(){
      return this->children;
  }
  void setOffset(int offset){
    this->offset = offset;
  }
   void setName(std::string name){
	std::strcpy(this->name, name.c_str());
   }
   void setLength(int length){
     this->length = length;
   }
};

class NaryTree{
  private:
    Node* root;
  public:
    NaryTree(char data[]): root(new Node(0, 0, data, "namespace")){
    }
    void printNodes(Node* node){
    if(node == nullptr){
        return;
      }
    for(Node* child: node ->getChildren()){
        printNodes(child);
      }
    }


  /*  void setFileRootPath(Node* root){
	    this->root = root;
    } */

    Node* findFileRootPath(){
	    return root;
    }
    
    void insert(Node* parent, Node* node){
	  if(parent == nullptr){
		  return;
	  }
	  if(node == nullptr){
		  return;
	  }
    if(root->getName() != "/"){
      root->setName("/");
    }
    parent->children.push_back(node);
    node->parent = parent;
    }

    Node* search(Node* node, std::vector<std::string> directory, size_t i = 0){
      if( i >= directory.size() || node == nullptr){
        return nullptr;
      }
      if(node->name == directory[directory.size()-1]){
	      return node;
      }
      if(directory[i] == node->name){
        if(i == directory.size()-1){
	        return node;
	      }
	      if(!node->children.empty()){
          for(Node* child : node->children){
            if(child->name == directory[i +1]){
              Node* result = search(child, directory, i + 1);
              if(result != nullptr){
                return result;
              }
	         }
	        }
        }
      }
      return nullptr;
    }

};

static std::vector<std::string> spotFinder(std::string directory){ //done

      std::vector<std::string> parsedString;
      
      size_t n = 0;
      std::string token;
      std::string slash = "/";

        if(directory.empty()){
        return parsedString;
        }

      parsedString.push_back("/");

        if(directory == "/"){
          return parsedString;
        }

      size_t first = 1;
      n = directory.find(slash);
      
        while (n != std::string::npos) {
          if(n > first)
          parsedString.push_back(directory.substr(first, n - first));
          first = n + 1;
          n  = directory.find(slash, first);
        }

        if(first < directory.length()){
          parsedString.push_back(directory.substr(first));
        }

      return parsedString;
}