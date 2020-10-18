#include "swift-lite.h"
#include "stdio.h"

FunctionNode::FunctionNode() : level(0), return_type("void") {}
Block::Block() : EntryNode(nullptr) {}
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
            {Block *block = new Block();
            block -> EntryNode = node;
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

class VariableNode
{
public:
    string type;
    vector <string> names;
    int level;
    VariableNode();
};

VariableNode::VariableNode() : type("Int") {}

static int funcIndex = 0;

void readVariablesGlobal(ASTNode* node,VariableNode *prev,int level){
    if (node){
        switch (node -> kind){
            case VAR_DEFINE:
                {
                    VariableNode *vn = new VariableNode();
                    vn -> level = level;
                    for (int i = 0; i < 2; i++)
                    {
                        readVariablesGlobal(node->ptr[i],vn,level);
                    }
                    
                    for (int i = 0; i < (vn->names).size(); i++)
                    {
                        string name = vn->names[i];
                        string type = vn->type;
                        Variable *variable = new Variable();
                        variable -> name = name;
                        variable -> type = type;
                        globalVars -> variables.insert(std::make_pair(level,variable));
                    }
                    
                }
                break;
            case VAR_LIST:
                {
                    ASTNode *temp = node;
                    while(temp){
                        
                        readVariablesGlobal(temp,prev,level);
                        temp = temp->ptr[1];
                    }
                }
                break;
            case TYPE:
                prev -> type = node->type;
                break;
            case ID:
                {
                    try
                    {
                        prev->names.push_back(get<string>(node->data));
                    }
                    catch(const std::exception& e)
                    {
                        printf("unexpetedly find no ID in variable list\n");
                    }
                    
                }
                break;
            case EXT_DEF_LIST:
                {
                    ASTNode *temp = node;
                    while(temp){
                        
                        readVariablesGlobal(temp,nullptr,level+1);
                        temp = temp->ptr[1];
                    }
                }
            case FUNCTION:
                {
                    FunctionNode *func = get_function_symbol(funcIndex);
                    readVariablesInBlock(func->block,globalVars);
                    funcIndex++;
                }
                break;
            default:
                break;
        }
    }
}

void readVariablesInBlock(Block *block,VariableList* father){

}
// void readVariablesInBlock(Block *block,VariableList* father){
//     #if PRINT_AST == 0
//     return;
//     #endif

//     if (node){

//         ASTNode *temp = node;
//         int index = 1;

//         switch ( (node->kind))
//         {
//         case ID:
//             printf("%*cID: %s\n", indent, ' ', get<string>(node->data).c_str());
//             break;
//         case INTEGER:
//             printf("%*cINTEGER: %d\n", indent, ' ', get<int>(node->data));
//             break;
//         case FLOAT:
//             printf("%*cFLOAT: %f\n", indent, ' ', get<float>(node->data));
//             break;
//         case CHAR:
//             printf("%*cCHAR: %c\n", indent, ' ', get<char>(node->data));
//             break;
//         case TYPE:
//             printf("%*cTYPE: %s\n", indent,' ', node->type.c_str());
//             break;
//         case COMPARE:
//             printf("%*cCOMPARE: %s\n", indent,' ', get<string>(node->data).c_str());
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case ASSIGN:
//             printf("%*c%s: \n", indent,' ', "ASSIGN");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case AND:
//             printf("%*c%s: \n", indent,' ', "AND");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case OR:
//             printf("%*c%s: \n", indent,' ', "OR");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case PLUS:
//             printf("%*c%s: \n", indent,' ', "PLUS");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case MINUS:
//             printf("%*c%s: \n", indent,' ', "MINUS");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case MULTI:
//             printf("%*c%s: \n", indent,' ', "MULTI");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case DIVID:
//             printf("%*c%s: \n", indent,' ', "DIVID");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case NOT:
//             printf("%*c%s: \n", indent,' ', "NOT");
//             print_sub_ast_nodes(node,indent,1);
//             break;
//         case INCREASE:
//             printf("%*c%s: \n", indent,' ', "INCREASE");
//             print_sub_ast_nodes(node,indent,1);
//             break;
//         case DECREASE: 
//             printf("%*c%s: \n", indent,' ', "DECREASE");
//             print_sub_ast_nodes(node,indent,1);
//             break;
//         case BREAK:
//             printf("%*c%s: \n", indent,' ', "BREAK");
//             break;
//         case CONTINUE:
//             printf("%*c%s: \n", indent,' ', "CONTINUE");
//             break;
//         case RETURN:
//             printf("%*c%s: \n", indent,' ', "RETURN");
//             print_sub_ast_nodes(node,indent,1);
//             break;
//         case FUNC_CALL:
//             printf("%*cFUNC_CALL:%s \n", indent,' ', get<string>(node->data).c_str());
//             print_sub_ast_nodes(node,indent,1);
//             break;
//         case ARGS:
//             printf("%*c%s: \n", indent,' ', "ARGS");
//             while (temp)
//             {
//                 printf("%*cArg[%d]:\n",indent + SPACECOUNT,' ',index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }
            
//             break;
//         case FUNC_ANNOUNCE:
//             printf("%*c%s: \n", indent,' ', "FUNC_ANNOUNCE");
//             print_sub_ast_nodes(node,indent,3);
//             break;
//         case DEFINE_LIST:
//             printf("%*c%s: \n", indent,' ', "DEFINE_LIST");
//             while (temp)
//             {
//                 printf("%*cArg[%d]:\n",indent + SPACECOUNT,' ',index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }
//             break;
//         case VAR_DEFINE:
//             printf("%*c%s: \n", indent,' ', "VAR_DEFINE");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case VAR_LIST:
//             printf("%*cVAR_LIST: \n", indent,' ');

//             while(temp){
//                 printf("%*cVAR_LIST[%d]: \n", indent + SPACECOUNT,' ', index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }

//             break;
//         case FUNC_PARAMETER:
//             printf("%*cFUNC_PARAMETER: \n", indent,' ');
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case FUNC_PARAMETERS:
//             printf("%*cFUNC_PARAMETERS: \n", indent,' ');

//             while(temp){
//                 printf("%*cPARAMETERS[%d]: \n", indent + SPACECOUNT,' ', index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }

//             break;
//         case STATEMENT_LIST:
//             printf("%*cSTATEMENT_LIST: \n", indent,' ');

//             while(temp){
//                 printf("%*cSTATEMENT_LIST[%d]: \n", indent + SPACECOUNT,' ', index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }

//             break;
//         case WHOLE_STATEMENT:
//             printf("%*c%s: \n", indent,' ', "WHOLE_STATEMENT");
//             print_sub_ast_nodes(node,indent,2);
//             break;

//         case Exp_STATMENT:
//             printf("%*c%s: \n", indent,' ', "Exp_STATMENT");
//             print_sub_ast_nodes(node,indent,1);
//             break;

//         case WHILE:
//             printf("%*c%s: \n", indent,' ', "WHILE");
//             print_sub_ast_nodes(node,indent,2);
//             break;

//         case IF_THEN_ELSE:
//             printf("%*c%s: \n", indent,' ', "IF_THEN_ELSE");
//             print_sub_ast_nodes(node,indent,3);
//             break;
//         case IF_THEN:
//             printf("%*c%s: \n", indent,' ', "IF_THEN");
//             print_sub_ast_nodes(node,indent,3);
//             break;
//         case FUNCTION:
//             printf("%*c%s: \n", indent,' ', "FUNCTION");
//             print_sub_ast_nodes(node,indent,2);
//             break;
//         case EXT_DEF_LIST:
//             printf("%*cEXT_DEF_LIST: \n", indent,' ');

//             while(temp){
//                 printf("%*cEXT_DEF_LIST[%d]: \n", indent + SPACECOUNT,' ', index);
//                 print_sub_ast_nodes(temp,indent,1);
//                 temp = temp->ptr[1];
//                 index++;
//             }

//             break;
//         default:
//             break;
//         }
//     }

// }


void readVariables(ASTNode *node){
    funcIndex = 0;
    readVariablesGlobal(node,nullptr,0);

}