#include "swift-lite.h"
#include "stdio.h"


std::unordered_map<std::string, FunctionNode*> functionMap;

void insertFunc(std::string name,FunctionNode* symbol){
    functionMap.insert(std::make_pair(name,symbol));
    printf("function inserted : %s\n",(symbol->name).c_str());
}

optional<FunctionNode*> search_function_symbol(const string &name){
    std::unordered_map<std::string, FunctionNode*>::iterator it;
    if ((it = functionMap.find(name)) != functionMap.end() ){
        return it->second;
    }
    return nullopt;
}