#include "swift-lite.h"
#include "parser.tab.h"
#include "stdio.h"

Operation::Operation() : data(0), kind(0), type(0), level(0) {}
CodeNode::CodeNode() : op(0), next(nullptr), prior(nullptr) {}
ASTNode::ASTNode() : kind(0), ptr(4, nullptr), idx(0), type(0), pos(0), num(0), code(nullptr) {}
Symbol::Symbol() : level(0), type(0), paramnum(0), flag(0), idx(0) {}

extern vector<Symbol> symbol_table;

ASTNode *make_node(int kind, int pos, vector<ASTNode *> nodes){
    
}