#include "swift-lite.h"
#include "stdio.h"

ASTNode::ASTNode() : kind(0), ptr(4, nullptr), idx(0), type(""), pos(0), num(0), code(nullptr) {}
Symbol::Symbol() : level(0), type(0), paramnum(0), flag(0), idx(0) {}
FunctionNode::FunctionNode() : level(0), return_type("void") {}
FuncBlock::FuncBlock() : node(nullptr) {}

void readFuncs(ASTNode *node,variant<Parameter*, FunctionNode*> prev);
void readSubNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count);

void readFuncs(ASTNode *node, variant<Parameter*, FunctionNode*> prev){

    if (node){

        ASTNode *temp = node;

        switch ( (node->kind)){
        
        case FUNC_ANNOUNCE:
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                readSubNodes(node,p,3);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find func announce outside functions");
            }
            
            break;
        case WHOLE_STATEMENT:
            FuncBlock *block = new FuncBlock;
            block -> node = node;
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                p -> block = block;
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find whole statements outside functions");
            } 
            break;
        case ID:
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                p -> name = get<string>(node -> data);
            }catch (std::bad_variant_access&) {
                try {
                    Parameter* p = get<Parameter*>(prev); 
                    p -> name = get<string>(node -> data);
                }catch (std::bad_variant_access&) {
                    printf("unexpectedly find ID");
                } 
            } 
            break;

        case FUNC_PARAMETERS:

            while(temp){
                readSubNodes(temp,prev,1);
                temp = temp->ptr[1];
            }

            break;
        case FUNC_PARAMETER:
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                Parameter *pa = new Parameter;
                (p -> parameters).push_back(pa);
                readSubNodes(node,pa,2);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find func parameters outside functions");
            }
            break;
        case TYPE:
            try {
                Parameter* p = get<Parameter*>(prev); 
                p -> type = get<string>(node->data);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find type outside parameters");
            }
            break;
        case FUNCTION:
            FunctionNode *func = new FunctionNode;
            readSubNodes(node,func,2);
            break;
        case EXT_DEF_LIST:
            while(temp){
                readSubNodes(node,prev,1);
                temp = temp->ptr[1];
            }
            break;
        default:
            break;
        }
    }

}

void readSubNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count){
    for (int i = 0;i < 4 && i<count;i++){
        if (node->ptr[i] == nullptr){
            //printf("unexpectedly found null ptr while printing node.\nCurrent Index is %d,expecting max index is %d.\n",i,count - 1);
            return;
        }
        readFuncs(node->ptr[i],prev);
    }
}

