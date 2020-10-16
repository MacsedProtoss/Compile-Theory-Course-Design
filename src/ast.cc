#include "swift-lite.h"
#include "stdio.h"

Operation::Operation() : data(0), kind(0), type(0), level(0) {}
CodeNode::CodeNode() : op(0), next(nullptr), prior(nullptr) {}
ASTNode::ASTNode() : kind(0), ptr(4, nullptr), idx(0), type(""), pos(0), num(0), code(nullptr) {}
Symbol::Symbol() : level(0), type(0), paramnum(0), flag(0), idx(0) {}

extern vector<Symbol> symbol_table;

void print_sub_ast_nodes(ASTNode *node, int indent,int count);
void print_ast_node(ASTNode *node, int indent);

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

%token  IF ELSE WHILE  FUNC FUNC_RETURN_TYPE COMMA EOL
%token FUNC_CALL FUNC_ANNOUNCE IF_THEN IF_THEN_ELSE Exp_STATMENT WHOLE_STATEMENT STATEMENT_LIST ARGS FUNC_PARAMETERS FUNC_PARAMETER 
%token VAR_LIST VAR_DEFINE DEFINE_ASSIGN DEFINE_LIST
*/

void print_sub_ast_nodes(ASTNode *node, int indent,int count){
    for (int i = 0;i < 4 && i<count;i++){
        if (node->ptr[i] == nullptr){
            printf("unexpectedly found null ptr while printing node.\nCurrent Index is %d,expecting max index is %d.\n",i,count - 1);
            return;
        }
        print_ast_node(node->ptr[i],indent + 2);
    }
}

void print_ast_node(ASTNode *node, int indent){
    #if PRINT_AST == 0
    return;
    #endif

    if (node){

        ASTNode *temp = node;
        int index = 1;

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
        case ASSIGN:
            printf("%*c%s: \n", indent,' ', "ASSIGN");
            print_sub_ast_nodes(node,indent,2);
            break;
        case AND:
            printf("%*c%s: \n", indent,' ', "AND");
            print_sub_ast_nodes(node,indent,2);
            break;
        case OR:
            printf("%*c%s: \n", indent,' ', "OR");
            print_sub_ast_nodes(node,indent,2);
            break;
        case PLUS:
            printf("%*c%s: \n", indent,' ', "PLUS");
            print_sub_ast_nodes(node,indent,2);
            break;
        case MINUS:
            printf("%*c%s: \n", indent,' ', "MINUS");
            print_sub_ast_nodes(node,indent,2);
            break;
        case MULTI:
            printf("%*c%s: \n", indent,' ', "MULTI");
            print_sub_ast_nodes(node,indent,2);
            break;
        case DIVID:
            printf("%*c%s: \n", indent,' ', "DIVID");
            print_sub_ast_nodes(node,indent,2);
            break;
        case NOT:
            printf("%*c%s: \n", indent,' ', "NOT");
            print_sub_ast_nodes(node,indent,1);
            break;
        case INCREASE:
            printf("%*c%s: \n", indent,' ', "INCREASE");
            print_sub_ast_nodes(node,indent,1);
            break;
        case DECREASE: 
            printf("%*c%s: \n", indent,' ', "DECREASE");
            print_sub_ast_nodes(node,indent,1);
            break;
        case BREAK:
            printf("%*c%s: \n", indent,' ', "BREAK");
            break;
        case CONTINUE:
            printf("%*c%s: \n", indent,' ', "CONTINUE");
            break;
        case RETURN:
            printf("%*c%s: \n", indent,' ', "RETURN");
            print_sub_ast_nodes(node,indent,1);
            break;
        case FUNC_CALL:
            printf("%*cFUNC_CALL:%s \n", indent,' ', get<string>(node->data).c_str());
            print_sub_ast_nodes(node,indent,1);
            break;
        case ARGS:
            printf("%*c%s: \n", indent,' ', "ARGS");
            while (temp)
            {
                printf("%*cArg[%d]:\n",indent + 2,' ',index);
                print_sub_ast_nodes(temp,indent,1);
                temp = temp->ptr[1];
                index++;
            }
            
            break;
        case FUNC_ANNOUNCE:
            printf("%*c%s: \n", indent,' ', "FUNC_ANNOUNCE");
            print_sub_ast_nodes(node,indent,3);
            break;
        case DEFINE_LIST:
            printf("%*c%s: \n", indent,' ', "DEFINE_LIST");
            print_sub_ast_nodes(node,indent,1);
            break;
        case VAR_DEFINE:
            printf("%*c%s: \n", indent,' ', "VAR_DEFINE");
            print_sub_ast_nodes(node,indent,2);
            break;
        case VAR_LIST:
            printf("%*cVAR_LIST: \n", indent,' ');

            while(temp){
                printf("%*cVAR_LIST[%d]: \n", indent+2,' ', index);
                print_sub_ast_nodes(temp,indent,1);
                temp = temp->ptr[1];
                index++;
            }

            break;
        case FUNC_PARAMETER:
            printf("%*cFUNC_PARAMETER: \n", indent,' ');
            print_sub_ast_nodes(node,indent,2);
            break;
        case FUNC_PARAMETERS:
            printf("%*cFUNC_PARAMETERS: \n", indent,' ');

            while(temp){
                printf("%*cPARAMETERS[%d]: \n", indent+2,' ', index);
                print_sub_ast_nodes(temp,indent,1);
                temp = temp->ptr[1];
                index++;
            }

            break;
        case STATEMENT_LIST:
            printf("%*cSTATEMENT_LIST: \n", indent,' ');

            while(temp){
                printf("%*cSTATEMENT_LIST[%d]: \n", indent+2,' ', index);
                print_sub_ast_nodes(temp,indent,1);
                temp = temp->ptr[1];
                index++;
            }

            break;
        case WHOLE_STATEMENT:
            printf("%*c%s: \n", indent,' ', "WHOLE_STATEMENT");
            print_sub_ast_nodes(node,indent,2);
            break;

        case Exp_STATMENT:
            printf("%*c%s: \n", indent,' ', "Exp_STATMENT");
            print_sub_ast_nodes(node,indent,1);
            break;

        case WHILE:
            printf("%*c%s: \n", indent,' ', "WHILE");
            print_sub_ast_nodes(node,indent,2);
            break;

        case IF_THEN_ELSE:
            printf("%*c%s: \n", indent,' ', "IF_THEN_ELSE");
            print_sub_ast_nodes(node,indent,3);
            break;
        case IF_THEN:
            printf("%*c%s: \n", indent,' ', "IF_THEN");
            print_sub_ast_nodes(node,indent,3);
            break;
        case FUNCTION:
            printf("%*c%s: \n", indent,' ', "FUNCTION");
            print_sub_ast_nodes(node,indent,2);
            break;
        case EXT_DEF_LIST:
            printf("%*cEXT_DEF_LIST: \n", indent,' ');

            while(temp){
                printf("%*cEXT_DEF_LIST[%d]: \n", indent+2,' ', index);
                print_sub_ast_nodes(temp,indent,1);
                temp = temp->ptr[1];
                index++;
            }

            break;
        default:
            break;
        }
    }

}

