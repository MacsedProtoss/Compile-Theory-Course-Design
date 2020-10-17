#include "swift-lite.h"
#include "stdio.h"

FunctionNode::FunctionNode() : level(0), return_type("void") {}
FuncBlockNode::FuncBlockNode() : node(nullptr) {}
Parameter::Parameter() : type("Int") {}

void readFuncs(ASTNode *node,variant<Parameter*, FunctionNode*> prev);
void readSubFunctionNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count);

void readFuncs(ASTNode *node, variant<Parameter*, FunctionNode*> prev){

    if (node){

        ASTNode *temp = node;

        switch ( (node->kind)){
        
        case FUNC_ANNOUNCE:
        
            {try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                readSubFunctionNodes(node,p,3);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find func announce outside functions\n");
            }}
            
            break;
        case WHOLE_STATEMENT:
            {FuncBlockNode *block = new FuncBlockNode();
            block -> node = node;
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                p -> block = block;
                insertFunc(p->name,p);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find whole statements outside functions\n");
            } }
            break;
        case ID:
            {
                bool flag = false;
                try {
                    FunctionNode* p = get<FunctionNode*>(prev); 
                    p -> name = get<string>(node -> data);
                    return;
                }catch (std::bad_variant_access&) {
                    flag = true;
                } 

                if (flag){
                    try {
                        Parameter* p = get<Parameter*>(prev); 
                        p -> name = get<string>(node -> data);
                        return;
                    }catch (std::bad_variant_access&) {
                        flag = true;
                    } 
                }

                if (flag){
                    printf("unexpectedly find ID");
                }

            }
            break;

        case FUNC_PARAMETERS:

            while(temp){
                readSubFunctionNodes(temp,prev,1);
                temp = temp->ptr[1];
            }

            break;
        case FUNC_PARAMETER:
            {try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                Parameter *pa = new Parameter();
                (p -> parameters).push_back(pa);
                readSubFunctionNodes(node,pa,2);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find func parameters outside functions\n");
            }}
            break;
        case TYPE:
            {
                int flag = 0;
                try {
                    Parameter* p = get<Parameter*>(prev); 
                    p -> type = node->type;
                    return;
                }catch (std::bad_variant_access& e) {
                    flag = 1;
                }
            
                if (flag == 1){
                    try {
                        FunctionNode* p = get<FunctionNode*>(prev); 
                        p -> return_type = node->type;
                        flag = 0;
                        return;
                    }catch (std::bad_variant_access&) {
                        flag = 1;
                    }
                }

                if (flag == 1){
                    printf("unexpectedly find type\n");
                }
            
            }
            
            break;
        case FUNCTION:
            {FunctionNode *func = new FunctionNode();
            readSubFunctionNodes(node,func,2);}
            break;
        case EXT_DEF_LIST:
            while(temp){
                readSubFunctionNodes(temp,prev,1);
                temp = temp->ptr[1];
            }
            break;
        default:

            break;
        }
    }

}

void readSubFunctionNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count){
    for (int i = 0;i < 4 && i<count;i++){
        if (node->ptr[i] == nullptr){
            //printf("unexpectedly found null ptr while printing node.\nCurrent Index is %d,expecting max index is %d.\n",i,count - 1);
            continue;
        }
        readFuncs(node->ptr[i],prev);
    }
}


void readVariables(ASTNode *node){
    
}
