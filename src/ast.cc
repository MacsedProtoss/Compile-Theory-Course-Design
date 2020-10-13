#include "swift-lite.h"
#include "stdio.h"

Operation::Operation() : data(0), kind(0), type(0), level(0) {}
CodeNode::CodeNode() : op(0), next(nullptr), prior(nullptr) {}
ASTNode::ASTNode() : kind(0), ptr(4, nullptr), idx(0), type(nullptr), pos(0), num(0), code(nullptr) {}
Symbol::Symbol() : level(0), type(0), paramnum(0), flag(0), idx(0) {}

extern vector<Symbol> symbol_table;

ASTNode *make_node(int kind, int pos, vector<ASTNode *> nodes){
    ASTNode *node = new ASTNode();
    node->kind = kind;
    node->pos = pos;

    int i = 0;
    for (i = 0;i < nodes.size();i++){
        node->ptr[i] = nodes[i];
    }

    while (i < 4)
    {
        i++;
        node->ptr[i] = nullptr;
    }
    

    return node;
}

/*
%type  <ptr> Exp STATEMENT WHOLESTATEMENT STATEMENTLIST Args PARAMETERS PARAMETER VARLIST VAR Specifier DEFINE DEFINEASSIGN DEFINELIST

%token NEW ANNOUNCE ASSIGN LBR RBR LCBR RCBR RETURN IF ELSE WHILE BREAK CONTINUE FUNC FUNC_RETURN_TYPE COMMA EOL
%token FUNC_CALL FUNC_ANNOUNCE IF_THEN IF_THEN_ELSE Exp_STATMENT WHOLE_STATEMENT STATEMENT_LIST ARGS FUNC_PARAMETERS FUNC_PARAMETER 
%token VAR_LIST VAR_DEFINE DEFINE_ASSIGN DEFINE_LIST
*/

void print_ast_node(ASTNode *node, int indent){
    #if PRINT_AST == 0
    return;
    #endif

    if (node){
        switch ( (node->kind))
        {
        case ID:
            printf("%*cID: %s\n", indent, ' ', get<string>(node->data).c_str());
            break;
        case INTEGER:
            printf("%*cINTEGER: %d\n", indent, ' ', get<int>(node->data));
            break;
        case FLOAT:
            printf("%*cFLOAT: %f\n", indent, ' ', get<float>(node->data));
            break;
        case CHAR:
            printf("%*cCHAR: %c\n", indent, ' ', get<char>(node->data));
            break;
        case TYPE:
            printf("%*cTYPE: %s\n", indent,' ', node->type.c_str());
            break;
        case COMPARE:
            printf("%*cCOMPARE: %s\n", indent,' ', get<string>(node->data).c_str());
            break;
        case AND:
        case OR:
        case PLUS:
        case MINUS:
        case MULTI:
        case DIVID:
            printf("%*c%s: \n", indent,' ', get<string>(node->data).c_str());
            print_ast_node(node->ptr[0],indent + 2);
            print_ast_node(node->ptr[1],indent + 2);
            break;
        case NOT:
        case INCREASE:
        case DECREASE: 
            printf("%*c%s: \n", indent,' ', get<string>(node->data).c_str());
            print_ast_node(node->ptr[0],indent + 2);
            break;
        
        default:
            break;
        }
    }

}