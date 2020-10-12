#define register

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"

#include "cstdio"
#include "string"
#include "unordered_map"
#include "vector"
#include "variant"
#include "tuple"
#include "optional"
#include "memory"

#include "parser.tab.h"

using namespace llvm;
using std::get, std::nullopt;
using std::string, std::unordered_map, std::vector, std::variant, std::shared_ptr, std::tuple, std::optional;

#define PRINT_AST 0
#define PRINT_SYMBOL_TABLE 0

class Operation{
public:
    int kind;
    int type;
    variant<int, float, string> data;
    int level;

    Operation();
};

class CodeNode{
public:
    int op;
    vector<shared_ptr<CodeNode>> data;
    Operation opn1, opn2, result;
    shared_ptr<CodeNode> next, prior;

    CodeNode();
};

class ASTNode{
public:
    int kind;
    variant<int, float, string,char> data;
    vector<ASTNode *> ptr;
    int idx;
    string Etrue, Efalse, Snext;
    shared_ptr<CodeNode> code;
    int type;
    int pos;
    int num;

    ASTNode();
};

class Symbol
{
public:
    string name;
    int level;
    int type;
    int paramnum;
    string alias;
    char flag;
    int idx;

    Symbol();
};

ASTNode *make_node(int kind, int pos, vector<ASTNode *> nodes = vector<ASTNode *>{});
void entrypoint(ASTNode *node);
void bool_expression(ASTNode *node);
void expression(ASTNode *node);
void print_llvm_ir(shared_ptr<CodeNode> head);
optional<Symbol> search_symbol_table_with_flag(const string &name, char flag);
tuple<Function *, FunctionType *, Function *, FunctionType *> inject_print_function(LLVMContext &ctx, IRBuilder<> &builder, Module &module);
void print_symbol_table();
