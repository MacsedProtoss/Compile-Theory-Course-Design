#include "swift-lite.h"
#include "stdio.h"


std::unordered_map<std::string, FunctionNode*> functionMap;
std::vector<FunctionNode*> functionArr;

void insertFunc(std::string name,FunctionNode* symbol){
    functionMap.insert(std::make_pair(name,symbol));
    functionArr.push_back(symbol);
    if(PRINT_SYMBOL_TABLE){
        printf("function inserted : %s\n",(symbol->name).c_str());
    }
}

optional<FunctionNode*> search_function_symbol(const string &name){
    std::unordered_map<std::string, FunctionNode*>::iterator it;
    if ((it = functionMap.find(name)) != functionMap.end() ){
        return it->second;
    }
    return nullopt;
}

FunctionNode* get_function_symbol(const int index){
    return functionArr.at(index);
}