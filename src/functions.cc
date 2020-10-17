#include "swift-lite.h"
#include "stdio.h"

Symbol::Symbol() : level(0), type(0), paramnum(0), flag(0), idx(0) {}

std::unordered_map<std::string, Symbol*> functionMap;

void insertFunc(std::string name,Symbol* symbol){
    functionMap.insert(std::make_pair(name,symbol));
}

optional<Symbol*> search_function_symbol(const string &name){
    std::unordered_map<std::string, Symbol*>::iterator it;
    if ((it = functionMap.find(name)) != functionMap.end() ){
        return it->second;
    }
    return nullopt;
}