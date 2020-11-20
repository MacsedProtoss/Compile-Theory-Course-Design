#include "swift-lite.h"

#include "string"
#include "iostream"
#include "unordered_map"
#include "vector"

using std::string, std::vector, std::to_string;
vector<Symbol> symbol_table;
vector<int> symbol_scope_chain_stack;

void entrypoint(ASTNode* node){
    {
        FunctionNode* pNode = new FunctionNode();
        Parameter *p = new Parameter();
        p -> type = Int;
        p -> name = "i";
        pNode -> name = "printI";
        pNode -> level = -1;
        pNode -> parameters.push_back(p);
        pNode->return_type = Void;
        insertFunc("printI",pNode);
    }
    {
        FunctionNode* pNode = new FunctionNode();
        Parameter *p = new Parameter();
        p -> type = Float;
        p -> name = "f";
        pNode -> name = "printf";
        pNode -> level = -1;
        pNode -> parameters.push_back(p);
        pNode->return_type = Void;
        insertFunc("printf",pNode);
    }

    readFuncs(node,(FunctionNode *)nullptr);
    readVaribales(node);
    return;
}

