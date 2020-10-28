#include "swift-lite.h"
#include "stdio.h"

FunctionNode::FunctionNode() : level(0), return_type("Void") {}
Block::Block() : EntryNode(nullptr) ,name(""),opt(nullptr){}
Parameter::Parameter() : type("Int") {}
Operation::Operation() : data(0), kind(0), type("Void"), level(0),next(nullptr) ,subOpts({}),blocks({}){}

void readFuncs(ASTNode *node,variant<Parameter*, FunctionNode*> prev);
void readSubFunctionNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count);
void checkParamters(ASTNode *node,FunctionNode* func,VariableList *list,int level);

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
                SemanticsError = true;
            }}
            
            break;
        case WHOLE_STATEMENT:
            {Block *block = new Block();
            block -> EntryNode = node;
            try {
                FunctionNode* p = get<FunctionNode*>(prev); 
                p -> block = block;
                block->name = p->name;
                insertFunc(p->name,p);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find whole statements outside functions\n");
                SemanticsError = true;
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
                    SemanticsError = true;
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
                SemanticsError = true;
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
                    SemanticsError = true;
                }
            
            }
            
            break;
        case FUNCTION:
            {
                FunctionNode *func = new FunctionNode();
                readSubFunctionNodes(node,func,2);
            }
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
    variant<char,int,float>data;
    bool hasValue;
    VariableNode();
};

VariableNode::VariableNode() : type("Int") ,hasValue(false) {}

static int funcIndex = 0;
void readVariablesInBlock(Block *block,VariableList* father,string name);
void readVariablesWithNode(ASTNode *node,VariableNode *prev,VariableList *list,int level);

void readVariablesGlobal(ASTNode* node,VariableNode *prev,int level){
    if (node){
        switch (node -> kind){
            case ASSIGN:
            {
                VariableNode *vn = new VariableNode();
                vn -> level = level;
                vn ->hasValue = true;
                readVariablesGlobal(node->ptr[0],vn,level);
                readVariablesWithNode(node,vn,globalVars,level);

                Operation *newOp = new Operation();
                newOp -> kind = DEFINE_ASSIGN;
                newOp -> data = vn -> data;
                newOp -> level = vn -> level;
                newOp -> type  = vn -> type;

                if (entryOperation == nullptr){
                    entryOperation = newOp;
                    currentOperation = entryOperation;
                }else{
                    currentOperation -> next = newOp;
                    currentOperation = newOp;
                }
            }
                
                break;
            case VAR_DEFINE:
                {
                    VariableNode *vn = prev;
                    if (prev == nullptr){
                        vn = new VariableNode();
                        vn -> level = level;
                    }
                    
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
                        if (vn -> hasValue){
                            variable ->value = vn -> data;
                        }
                        globalVars -> variables.insert(std::make_pair(level,variable));
                    }


                    Operation *newOp = new Operation();
                    newOp -> kind = VAR_DEFINE;
                    newOp ->level = vn -> level;
                    newOp ->type = vn -> type;
                    if (prev == nullptr){
                        
                        if (entryOperation == nullptr){
                            entryOperation = newOp;
                            currentOperation = entryOperation;
                        }else{
                            currentOperation -> next = newOp;
                            currentOperation = newOp;
                        }
                    }else{
                        currentOperation ->subOpts.push_back(newOp);
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
                        SemanticsError = true;
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
                    
                    Operation *newOp = new Operation();
                    newOp ->kind = FUNC_ANNOUNCE;
                    newOp ->level = func -> level;
                    newOp ->type = func ->return_type;
                    
                    if (entryOperation == nullptr){
                        entryOperation = newOp;
                        currentOperation = entryOperation;
                    }else{
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    readVariablesInBlock(func->block,globalVars,func->name);
                    funcIndex++;
                }
                break;
            
            default:
                break;
        }
    }
}

// void readVariablesInBlock(Block *block,VariableList* father){

// }
void readVariablesWithNode(ASTNode *node,VariableNode *prev,VariableList *list,int level){
    if (node){
        switch(node -> kind){
            case IF_THEN:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation ->kind = IF_THEN;
                        currentOperation ->level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = IF_THEN;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    for (int i = 0;i < 2;i++){
                        readVariablesWithNode(node->ptr[i],nullptr,list,level);
                    }
                
                }
                
                break;
            case IF_THEN_ELSE:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation ->kind = IF_THEN_ELSE;
                        currentOperation ->level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = IF_THEN_ELSE;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    for (int i = 0;i < 3;i++){
                        readVariablesWithNode(node->ptr[i],nullptr,list,level);
                    }
                
                }
                break;
            case WHILE:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation ->kind = WHILE;
                        currentOperation ->level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = WHILE;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    for (int i = 0;i < 2;i++){
                        readVariablesWithNode(node->ptr[i],nullptr,list,level);
                    }
                
                }
                break;
            case WHOLE_STATEMENT:
                {
                    Block *block = new Block();
                    block -> EntryNode = node;
                    currentOperation -> blocks.push_back(block);
                    block -> name = "INFUNC";
                    readVariablesInBlock(block,list,"INFUNC");
                }
                break;
            case CONTINUE:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation -> kind = CONTINUE;
                        currentOperation -> level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = CONTINUE;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }
                }
                break;
            case RETURN:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation -> kind = RETURN;
                        currentOperation -> level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = RETURN;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    readVariablesWithNode(node->ptr[0],nullptr,list,level);
                }
                break;
            case FUNC_CALL:
                {
                    if (currentOperation ->kind == 0){
                        currentOperation -> kind = FUNC_CALL;
                        currentOperation -> level = level;
                    }else{
                        Operation *newOp = new Operation();
                        newOp -> kind = FUNC_CALL;
                        newOp -> level = level;
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }

                    bool flag = false;
                    FunctionNode *func = nullptr;
                    for (int i = 0; i < funcIndex; i++){
                        FunctionNode *funcNode = get_function_symbol(i);
                        if (funcNode ->name == get<string>(node->data)){
                            flag = true;
                            func = funcNode;
                            break;
                        }
                        
                    }

                    if (flag){
                        
                    }else{
                        printf("Symbol not defined !");
                        SemanticsError = true;
                    }

                }
                break;
            case Exp_STATMENT:
                break;
            case VAR_DEFINE:
                break;
            case ASSIGN:
                {
                    if (prev != nullptr){
                        printf("unepectedly found assign inside 'exp assign exp'");
                        SemanticsError = true;
                    }else{
                        VariableNode *vn = new VariableNode();
                        vn -> level = level;
                        vn -> hasValue = true;
                        for (int i = 0;i < 2;i++){
                            readVariablesWithNode(node->ptr[i],vn,list,level);
                        }
                    }
                }
                break;
            case AND:

                break;
            case OR:
                break;
            case COMPARE:
                break;
            case PLUS:
                break;
            case MINUS:
                break;
            case MULTI:
                break;
            case DIVID:
                break;
            case NOT:
                break;
            case INCREASE:
                break;
            case DECREASE:
                break;
            case ID:
                break;
            case INTEGER:
                break;
            case FLOAT:
                break;
            case CHAR:
                break;
            default:
                break;
        }
    }
}

void readVariablesInBlock(Block *block,VariableList* father,string name){
    ASTNode *node = block -> EntryNode;
    VariableList *list = new VariableList();
    list ->namespacing = name;
    list -> father = father;
    block ->varlist = list;

    Operation *prevOpt = currentOperation;

    Operation *newOp = new Operation();
    block ->opt = newOp;
    currentOperation = newOp;

    if (node){

        switch ( (node->kind))
        {
        
        case STATEMENT_LIST:
            
            {

                int index = 0;
                ASTNode *temp = node;
                while(temp){
                    readVariablesWithNode(temp,nullptr,list,index);
                    temp = temp->ptr[1];
                    index ++;
                }
                currentOperation = prevOpt;
            }

            break;
        default:
            break;
        }
    }

}

void checkParamters(ASTNode *node,FunctionNode* func,VariableList *list,int level){
    ASTNode *temp = node;
    for (int i = 0; i < func->parameters.size(); i++)
    {
        Parameter *param = func->parameters[i];
        ASTNode *arg = temp->ptr[0];
        if (arg == nullptr){
            printf("expecting arg count is %d, but found null at %d", func->parameters.size(),i+1);
            SemanticsError = true;
            return;
        }else{
            switch (arg->kind)
            {
            case ID:
                if (auto ret = search_variable_symbol(get<string>(node->data),level,list)){
                    if (param->type == ret.value()->type)
                    {
                        
                    }else{
                        printf("unexpected arg type at %d",i+1);
                        SemanticsError = true;
                    }
                    
                }else{
                    printf("can't find this arg ID at %d",i+1);
                    SemanticsError = true;
                }
                break;
            case INTEGER:

                break;
            case FLOAT:
                break;
            case CHAR:
                break;
            default:
                printf("unexpected arg type at %d",i+1);
                SemanticsError = true;
                break;
            }
            
        }
        
        temp = temp->ptr[1];

    }
    
}


void readVariables(ASTNode *node){
    funcIndex = 0;
    readVariablesGlobal(node,nullptr,0);

}