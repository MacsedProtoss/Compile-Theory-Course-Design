#include "swift-lite.h"

#include "string"
#include "iostream"
#include "unordered_map"
#include "vector"

using std::string, std::vector, std::to_string;
vector<Symbol> symbol_table;
vector<int> symbol_scope_chain_stack;

void entrypoint(ASTNode* node){
    readFuncs(node,(FunctionNode *)nullptr);
    readVaribales(node);
    return;
}

