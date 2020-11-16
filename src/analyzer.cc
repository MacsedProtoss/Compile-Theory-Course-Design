#include "swift-lite.h"
#include "stdio.h"

class VariableNode
{
public:
    optType type;
    vector <string> names;
    int level;
    variant<char,int,float>data;
    bool hasValue;
    VariableNode();
};
VariableNode::VariableNode() : type(Void) ,hasValue(false) {}

FunctionNode::FunctionNode() : level(0), return_type(Void) {}
Block::Block() : EntryNode(nullptr) ,name(""),opt(nullptr){}
Parameter::Parameter() : type(Void) {}
Operation::Operation() : kind(0), level(0),next(nullptr),return_type(Void){}
DefineOpt::DefineOpt() : names({}),type(Void) {}
NormalOpt::NormalOpt() : left(nullptr) {}
CompareOpt::CompareOpt() : type(Equal) {}
RightOpt::RightOpt() : right(nullptr) {}
VarUseOpt::VarUseOpt() : type(Void), name("") {}
StaticValueOpt::StaticValueOpt() : type(Void), data(0) {}
FuncCallOpt::FuncCallOpt() : func(nullptr),args({}) {}
ConditionOpt::ConditionOpt() : condition(nullptr) {}
IfOpt::IfOpt() : condintion(nullptr),ifBlock(nullptr),elseBlock(nullptr) {}
WhileOpt::WhileOpt() : condintion(nullptr),ifBlock(nullptr) {}
FuncAnnounceOpt::FuncAnnounceOpt() : func(nullptr){}


VariableList *globalVars;
int funcIndex = 0;
bool SemanticsError = false;
vector<int> whileStack;
Operation *entryOperation;
Operation *currentOperation;

Operation *readVariablesWithNode(ASTNode *node,VariableList *list,int level);
Operation *readVarDefineOpt(ASTNode* node,VariableNode* var,VariableList* list,int level); // var a,b = 1, read var a,b
Operation *readAssignRightOpt(ASTNode* node,VariableList* list,int level,bool isGlobal);// lExp = rExp, read rExp
Operation *readAssignLeftOpt(ASTNode* node,VariableList* list,int level);// lExp = rExp, read lExp
Operation *readSimpleOpt(ASTNode* node,VariableList* list,int prevKind,int level,bool isGlobal);// a > b , !b , a != b , read a & b

void readFuncs(ASTNode *node,variant<Parameter*, FunctionNode*> prev);
void readSubFunctionNodes(ASTNode *node,variant<Parameter*, FunctionNode*> prev,int count);
void checkParamters(ASTNode *node,FunctionNode* func,VariableList *list,int level);
void readVariablesInBlock(Block *block,VariableList* father,string name);
optType getOptType(string raw);
compareType getCompareType(string raw);
variant<char,int,float> getStaticValue(variant<int,float,string,char> origin,optType type);

void readFuncs(ASTNode *node, variant<Parameter*, FunctionNode*> prev){

    if (node){

        ASTNode *temp = node;

        switch ( (node->kind)){
        
        case FUNC_ANNOUNCE:
            {try {
                FunctionNode* p = get<FunctionNode*>(prev);
                readSubFunctionNodes(node,p,3);
            }catch (std::bad_variant_access&) {
                printf("unexpectedly find func announce outside functions, at line %d\n",node->pos);
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
                printf("unexpectedly find whole statements outside functions, at line %d\n",node -> pos);
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
                    printf("unexpectedly find ID, at line %d\n",node -> pos);
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
                printf("unexpectedly find func parameters outside functions, at line %d\n",node -> pos);
                SemanticsError = true;
            }}
            break;
        case TYPE:
            {
                int flag = 0;
                try {
                    Parameter* p = get<Parameter*>(prev); 
                    p -> type = getOptType(node->type);
                    return;
                }catch (std::bad_variant_access& e) {
                    flag = 1;
                }
            
                if (flag == 1){
                    try {
                        FunctionNode* p = get<FunctionNode*>(prev); 
                        p -> return_type = getOptType(node->type);
                        flag = 0;
                        return;
                    }catch (std::bad_variant_access&) {
                        flag = 1;
                    }
                }

                if (flag == 1){
                    printf("unexpectedly find type, at line %d\n",node -> pos);
                    SemanticsError = true;
                }
            
            }
            
            break;
        case FUNCTION:
            {
                FunctionNode *func = new FunctionNode();
                readSubFunctionNodes(node,func,2);
                func -> block -> varlist = new VariableList();
                func -> block -> varlist -> father = globalVars;
                func -> block -> varlist -> namespacing = func -> name;
                vector<Variable *> vars;
                for (int i = 0; i < func->parameters.size(); i++)
                {
                    Parameter *pa = func->parameters[i];
                    string name = pa ->name;
                    optType type = pa->type;
                    Variable *variable = new Variable();
                    variable -> name = name;
                    variable -> type = type;
                    for (int j = 0; j < vars.size(); j++)
                    {
                        Variable* var = vars[j];
                        if (var->name == variable -> name)
                        {
                            printf("unexpectedly find conflict arg (args[%d]) with ID:%s, at line %d\n",j,variable->name.c_str(),node -> pos);
                            SemanticsError = true;
                        }
                        
                    }
                    
                    vars.push_back(variable);
                }
                func -> block ->varlist->variables.insert(std::make_pair(-1,vars));
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

void readVariablesGlobal(ASTNode* node,int level){
    if (node){
        switch (node -> kind){
            case ASSIGN:
            {
                
                NormalOpt *newOp = new NormalOpt();
                newOp -> kind = ASSIGN;
                newOp -> left = readVarDefineOpt(node->ptr[0],nullptr,globalVars,level);
                newOp -> right = readAssignRightOpt(node->ptr[1],globalVars,level,true);

                if (newOp -> left ->return_type != newOp -> right -> return_type)
                {
                    if (newOp -> left -> return_type == Float && newOp -> right -> return_type == Int)
                    {
                        newOp -> return_type = Float;
                    }else if (newOp -> left -> return_type == Int && newOp -> right ->return_type == Char)
                    {
                        newOp -> return_type = Int;
                    }else if (newOp -> left -> return_type == Char && newOp -> right ->return_type == Int)
                    {
                        newOp -> return_type = Char;
                    }else{
                        printf("assign type conflict: left is %d(enum) , right is %d(enum) , at line %d\n",newOp->left->return_type,newOp->right->return_type,node -> pos);
                        SemanticsError = true;
                        return;
                    }
                    
                }else{
                    newOp -> return_type = newOp -> left -> return_type;
                }

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
                    Operation *newOp = readVarDefineOpt(node,nullptr,globalVars,level);
                    if (entryOperation == nullptr){
                        entryOperation = newOp;
                        currentOperation = entryOperation;
                    }else{
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }
                }
                break;
            case EXT_DEF_LIST:
                {
                    ASTNode *temp = node;
                    int l = level;
                    while(temp){
                        readVariablesGlobal(temp->ptr[0],l);
                        temp = temp->ptr[1];
                        l++;                  
                    }
                }
                break;
            case FUNCTION:
                {
                    FunctionNode *func = get_function_symbol(funcIndex);

                    readVariablesInBlock(func->block,globalVars,func->name);
                    funcIndex++;

                    FuncAnnounceOpt *newOp = new FuncAnnounceOpt();
                    newOp -> kind = FUNCTION;
                    newOp -> func = func;
                    newOp -> level = level;
                    newOp -> return_type = func -> return_type;

                    if (entryOperation == nullptr){
                        entryOperation = newOp;
                        currentOperation = entryOperation;
                    }else{
                        currentOperation -> next = newOp;
                        currentOperation = newOp;
                    }
                    
                }
                break;
            
            default:
                break;
        }
    }
}

Operation *readAssignRightOpt(ASTNode* node,VariableList* list,int level,bool isGlobal){
    if (node)
    {
        switch (node->kind)
        {
        case ID: case INTEGER :case FLOAT: case CHAR: case COMPARE: case AND: case OR: case PLUS: case MINUS: case MULTI: case DIVID: case FUNC_CALL:
            {
                if (isGlobal && node->kind == ID)
                {
                    printf("initializer element is not constant, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                return readSimpleOpt(node,list,ASSIGN,level,isGlobal);
            }
            break;
        case ASSIGN:
            printf("unexpectedly find ASSIGN right of ASSIGN, at line %d\n",node -> pos);
            SemanticsError = true;
            return nullptr;
        
        default:
            printf("unexpecting node type at assign right of %d(enum), at line %d\n",node -> kind,node -> pos);
            SemanticsError = true;
            return nullptr;
            break;
        }
    }else{
        printf("unexpectedly found null node!, at line %d\n",node -> pos);
        SemanticsError = true;
        return nullptr;
    }
    
    return nullptr;
}

Operation *readAssignLeftOpt(ASTNode* node,VariableList* list,int level){
    if (node)
    {
        switch (node->kind)
        {
        case VAR_DEFINE:
            {
                return readVarDefineOpt(node,nullptr,list,level);
            }
            break;
        case ID:
            {
                return readSimpleOpt(node,list,ASSIGN,level,false);
            }
            break;
        case ASSIGN:
            printf("unexpectedly find ASSIGN left of ASSIGN, at line %d\n",node -> pos);
            SemanticsError = true;
            return nullptr;
        
        default:
            printf("unexpecting node type at assign left of %d(enum), at line %d\n",node -> kind,node -> pos);
            SemanticsError = true;
            return nullptr;
            break;
        }
    }else{
        printf("unexpectedly found null node!, at line %d\n",node -> pos);
        SemanticsError = true;
        return nullptr;
    }
    
    return nullptr;
}

Operation *readSimpleOpt(ASTNode* node,VariableList* list,int prevKind,int level,bool isGlobal){
    if (node)
    {
        switch (node->kind)
        {
        case ID:
            {
                if (isGlobal)
                {
                    printf("initializer element is not constant, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                if (auto ret = search_variable_symbol(get<string>(node->data),level,list)){
                    
                    VarUseOpt *newOp = new VarUseOpt();
                    newOp->kind = ID;
                    newOp -> type = ret.value()->type;
                    newOp -> name = get<string>(node->data);
                    newOp -> return_type = newOp -> type;
                    return newOp;
    
                }else{
                    printf("Undefined variable: %s,at line %d\n",get<string>(node->data).c_str(),node->pos);
                    SemanticsError = true;
                    return nullptr;
                }
            }
            break;
        case INTEGER :case FLOAT: case CHAR:
            {
                StaticValueOpt *newOp = new StaticValueOpt();
                newOp -> kind = node -> kind;
                newOp -> type = getOptType(node->type);
                newOp -> data = getStaticValue(node->data,newOp -> type);
                newOp -> return_type = newOp -> type;
                return newOp;
            }
            break;
        case NOT: case INCREASE: case DECREASE:
            {
                if (node->kind == NOT && prevKind == NOT)
                {
                    printf("unexpectedly found !! Statement, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                
                RightOpt *newOp = new RightOpt();
                newOp -> kind = node -> kind;
                newOp -> right = readSimpleOpt(node->ptr[0],list,node -> kind,level,isGlobal);
                if (node -> kind == NOT)
                {
                    if (newOp ->right->return_type == Void)
                    {
                        printf("excuting NOT operation to Void type is not accepted, at line %d\n",node -> pos);
                        SemanticsError = true;
                        return nullptr;
                    }
                    
                }else{
                    if (newOp ->right->return_type != Int)
                    {
                        printf("INCREASE/DECREASE can only be applied to Int, at line %d\n",node -> pos);
                        SemanticsError = true;
                        return nullptr;
                    }else if (newOp -> right -> kind == Int){
                        printf("INCREASE/DECREASE can only be applied to Varable but not static Int, at line %d\n",node -> pos);
                        SemanticsError = true;
                        return nullptr;
                    }
                    
                }
                
                newOp -> return_type = Int;
                return newOp;
            }
        case COMPARE:
            {
                if (prevKind == COMPARE)
                {
                    printf("unexpectedly found Compare after Compare, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                
                CompareOpt *newOp = new CompareOpt();
                newOp -> kind = COMPARE;
                newOp -> type = getCompareType(node->type);
                newOp -> left = readSimpleOpt(node->ptr[0],list,node -> kind,level,isGlobal); 
                newOp -> right = readSimpleOpt(node->ptr[1],list,node -> kind,level,isGlobal); 
                if (newOp -> left -> return_type == Void || newOp -> right -> return_type == Void)
                {
                    printf("trying to excute conditions between Void type is not accepted, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                newOp -> return_type = Int;
                return newOp;
            }
            break;
        case AND: case OR: case PLUS: case MINUS: case MULTI: case DIVID:
            {
                NormalOpt *newOp = new NormalOpt();
                newOp -> kind = node -> kind;
                newOp -> left = readSimpleOpt(node->ptr[0],list,node -> kind,level,isGlobal); 
                newOp -> right = readSimpleOpt(node->ptr[1],list,node -> kind,level,isGlobal); 
                if (node -> kind == AND || node -> kind == OR)
                {
                    if (newOp -> left -> return_type == Void || newOp -> right -> return_type == Void)
                    {
                        printf("trying to excute conditions between Void type is not accepted, at line %d\n",node -> pos);
                        SemanticsError = true;
                        return nullptr;
                    }
                    
                    newOp -> return_type = Int;
                }else{
                    if (newOp -> left -> return_type != newOp -> right -> return_type)
                    {
                        if (newOp -> left -> return_type == Char || newOp -> right -> return_type == Char || newOp -> left -> return_type == Void || newOp -> right -> return_type == Void)
                        {
                            printf("conflict types, at line %d\n",node -> pos);
                            SemanticsError = true;
                            return nullptr;
                        }

                        newOp -> return_type = Float;
                    }else{
                        newOp -> return_type = newOp -> left -> return_type;
                    }
                    
                }
                return newOp;
            }
            break;
        case FUNC_CALL:
            {
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
                    checkParamters(node->ptr[0],func,list,level);
                }else{
                    printf("Func not defined !, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }

                FuncCallOpt *newOp = new FuncCallOpt();
                newOp -> kind = node -> kind;
                newOp -> func = func;
                newOp -> level = level;
                newOp -> return_type = func -> return_type;
                return newOp;
                
            }
            break;
        default:
            printf("unexpecting node type at simple opt of %d(enum), at line %d\n",node -> kind,node -> pos);
            SemanticsError = true;
            return nullptr;
            break;
        }
    }else{
        printf("unexpectedly found null node!, at line %d\n",node -> pos);
        SemanticsError = true;
        return nullptr;
    }
    
    return nullptr;
}

Operation *readVarDefineOpt(ASTNode* node,VariableNode* var,VariableList* list,int level){
    if (node)
    {
        switch (node->kind)
        {
        case VAR_DEFINE:
                {
                    VariableNode *vn = new VariableNode();
                    vn -> level = level;
                    
                    for (int i = 0; i < 2; i++)
                    {
                        readVarDefineOpt(node->ptr[i],vn,list,level);
                    }
                    
                    vector<Variable*> vars;
                    for (int i = 0; i < (vn->names).size(); i++)
                    {
                        string name = vn->names[i];
                        optType type = vn->type;
                        Variable *variable = new Variable();
                        variable -> name = name;
                        variable -> type = type;
                        if (vn -> hasValue){
                            variable ->value = vn -> data;
                        }

                        for (int j = 0; j < vars.size(); j++)
                        {
                            Variable* var = vars[j];
                            if (var->name == variable -> name)
                            {
                                printf("unexpectedly find conflict arg (args[%d]) with ID:%s, at line %d\n",j,variable->name.c_str(),node -> pos);
                                SemanticsError = true;
                            }
                            
                        }

                        vars.push_back(variable);
                    }
                    list -> variables.insert(std::make_pair(level,vars));

                    DefineOpt *newOp = new DefineOpt();
                    newOp -> kind = VAR_DEFINE;
                    newOp ->level = vn -> level;
                    newOp ->type = vn -> type;
                    newOp -> names = vn -> names;
                    newOp -> return_type = newOp -> type;
                    return (Operation *)newOp;
                    
                }
                break;
        case VAR_LIST:
                {
                    ASTNode *temp = node;
                    while(temp){
                        
                        readVarDefineOpt(temp->ptr[0],var,list,level);
                        temp = temp->ptr[1];
                    }
                }
                break;
        case ID:
                {
                    try
                    {
                        var->names.push_back(get<string>(node->data));
                    }
                    catch(const std::exception& e)
                    {
                        printf("unexpetedly find no ID in variable list, at line %d\n",node -> pos);
                        SemanticsError = true;
                    }
                    
                }
                break;
        case TYPE:
                var -> type = getOptType(node->type);
                break;
        default:
            return nullptr;
            break;
        }
    }else{
        return nullptr;
    }
    
    return nullptr;
}

Operation* readVariablesWithNode(ASTNode *node,VariableList *list,int level){
    if (node){
        switch(node -> kind){
            case IF_THEN:
                {

                    IfOpt *newOp = new IfOpt();
                    newOp -> kind = IF_THEN;
                    newOp -> level = level;

                    ConditionOpt *condition = new ConditionOpt();
                    condition -> level = level;
                    condition -> condition = readSimpleOpt(node->ptr[0],list,IF_THEN,level,false);
                    
                    newOp -> condintion = condition;
                    
                    Block *ifBlock = new Block();
                    ifBlock -> EntryNode = node->ptr[1] -> ptr[0];
                    
                    readVariablesInBlock(ifBlock,list,"INFUNC_IF_THEN");

                    newOp -> ifBlock = ifBlock;
                    return newOp;
                
                }
                
                break;
            case IF_THEN_ELSE:
                {

                    IfOpt *newOp = new IfOpt();
                    newOp -> kind = IF_THEN_ELSE;
                    newOp -> level = level;

                    ConditionOpt *condition = new ConditionOpt();
                    condition -> level = level;
                    condition -> condition = readSimpleOpt(node->ptr[0],list,IF_THEN,level,false);

                    newOp -> condintion = condition;
                    
                    Block *ifBlock = new Block();
                    Block *elesBlock = new Block();
                    ifBlock -> EntryNode = node->ptr[1] -> ptr[0];
                    elesBlock -> EntryNode = node -> ptr[2] -> ptr[0];
                    
                    readVariablesInBlock(ifBlock,list,"INFUNC_IF_THEN_ELSE");
                    readVariablesInBlock(elesBlock,list,"INFUNC_IF_THEN_ELSE");
                    newOp -> ifBlock = ifBlock;
                    newOp -> elseBlock = elesBlock;
                    return newOp;
                
                }
                break;
            case WHILE:
                {
                    whileStack.push_back(1);
                    WhileOpt *newOp = new WhileOpt();
                    newOp -> kind = WHILE;
                    newOp -> level = level;

                    ConditionOpt *condition = new ConditionOpt();
                    condition -> level = level;
                    condition -> condition = readSimpleOpt(node->ptr[0],list,IF_THEN,level,false);

                    newOp -> condintion = condition;
                    
                    Block *ifBlock = new Block();
                    ifBlock -> EntryNode = node->ptr[1]->ptr[0];
                    
                    readVariablesInBlock(ifBlock,list,"INFUNC_WHILE");

                    newOp -> ifBlock = ifBlock;
                    whileStack.pop_back();
                    return newOp;
                
                }
                break;
            case WHOLE_STATEMENT:
                {
                    printf("unexpectedly found WHOLE STATEMENT which is not predicted！, at line %d\n",node -> pos);
                    SemanticsError = true;
                    return nullptr;
                }
                break;
            case CONTINUE: case BREAK:
                {
                    if (whileStack.size() == 0)
                    {
                        printf("unexpectedly found continue/break outside of while, at line %d\n",node -> pos);
                        SemanticsError = true;
                        return nullptr;
                    }
                    
                    Operation *newOp = new Operation();
                    newOp -> kind = node -> kind;
                    newOp -> level = level;
                    newOp -> return_type = Void;
                    return newOp;
                    
                }
                break;
            case RETURN:
                {
                    RightOpt *newOp = new RightOpt();
                    newOp -> kind = RETURN;
                    newOp -> level = level;
                    if (!node->ptr[0])
                    {
                        newOp -> return_type = Void;
                    }else{
                        newOp -> right = readSimpleOpt(node->ptr[0],list,ASSIGN,level,false);
                        newOp -> return_type = newOp -> right -> return_type;
                    }
                    
                    FunctionNode *func = get_function_symbol(funcIndex);
                    
                    if (func -> return_type != newOp -> return_type)
                    {
                        if (func -> return_type == Float && newOp -> return_type == Int)
                        {
                            newOp -> return_type = Float;
                        }else if (func -> return_type == Int && newOp -> return_type == Char){
                            newOp -> return_type = Int;
                        }else{
                            printf("func return type error! expecting %d(enum) but found %d(enum), at line %d\n",func -> return_type ,newOp -> return_type,node -> pos);
                            SemanticsError = true;
                            return nullptr;
                        }
                        
                    }
                    return newOp;
                }
                break;
            case Exp_STATMENT:
                {
                    Operation *newOp = readVariablesWithNode(node->ptr[0],list,level);
                    return newOp;
                }
                
                break;
            case VAR_DEFINE:
                {
                    Operation *newOp = readVarDefineOpt(node,nullptr,list,level);
                    return newOp;
                }
                break;
            case ASSIGN:
                {
                
                    NormalOpt *newOp = new NormalOpt();
                    newOp -> kind = ASSIGN;
                    newOp -> left = readAssignLeftOpt(node->ptr[0],list,level);
                    newOp -> right = readAssignRightOpt(node->ptr[1],list,level,false);
                    if (newOp -> left ->return_type != newOp -> right -> return_type)
                    {
                        if (newOp -> left -> return_type == Float && newOp -> right -> return_type == Int)
                        {
                            newOp -> return_type = Float;
                        }else if (newOp -> left -> return_type == Int && newOp -> right ->return_type == Char)
                        {
                            newOp -> return_type = Int;
                        }else if (newOp -> left -> return_type == Char && newOp -> right ->return_type == Int)
                        {
                            newOp -> return_type = Char;
                        }else{
                            printf("assign type conflict: left is %d(enum) , right is %d(enum) , at line %d\n",newOp -> left ->return_type,newOp->right->return_type,node -> pos);
                            SemanticsError = true;
                            return nullptr;
                        }
                        
                    }else{
                        newOp -> return_type = newOp -> left -> return_type;
                    }
                    return newOp;
                }
                break;
            case AND: case OR: case NOT: case PLUS: case MINUS: case MULTI: case DIVID:
            case COMPARE: case INCREASE: case DECREASE:
            case INTEGER: case FLOAT: case CHAR: case ID: case FUNC_CALL:
                {
                    Operation *newOp = readSimpleOpt(node,list,STATEMENT_LIST,level,false);
                    return newOp;
                }
                break;
            default:
                return nullptr;
                break;
        }
    }
    return nullptr;
}

void readVariablesInBlock(Block *block,VariableList* father,string name){
    ASTNode *node = block -> EntryNode;
    VariableList *list;
    if (father == globalVars)
    {
        list = block -> varlist;
    }else{
        list = new VariableList();
        list ->namespacing = name;
        list -> father = father;
        block -> varlist = list;
    }
    block -> name = name;
    block -> opt = nullptr;
    if (node){

        switch ( (node->kind))
        {
        
        case WHOLE_STATEMENT:
            {
                Operation *tempOpt = nullptr;
                Operation *firstOpt = nullptr;
                int index = 0;
                ASTNode *temp = node->ptr[0];
                while(temp){
                    Operation *newOp = readVariablesWithNode(temp->ptr[0],list,index);
                    if (newOp)
                    {
                        if (firstOpt)
                        {
                            tempOpt -> next = newOp;
                            tempOpt = newOp;
                        }else{
                            firstOpt = newOp;
                            tempOpt = newOp;
                            printf("block %s first opt built with kind %d\n",block->name.c_str(),firstOpt -> kind);
                        }
                    }
                    
                    temp = temp->ptr[1];
                    index ++;
                }

                if (firstOpt)
                {
                    if (firstOpt -> kind < 0 || firstOpt -> kind > 400)
                    {
                        int tempIndex = 0;
                        auto newNode = node->ptr[0];
                        auto newTemp = readVariablesWithNode(newNode->ptr[0],list,0);
                        while (tempIndex < index && !newTemp)
                        {
                            newNode = newNode -> ptr[1];
                            tempIndex ++;
                            newTemp = readVariablesWithNode(newNode->ptr[0],list,tempIndex);
                        }
                        
                        
                        newTemp -> next = firstOpt -> next;
                        firstOpt = newTemp;
                    }
                    
                }
                

                block -> opt = firstOpt;
            }

            if (block -> opt)
            {
                printf("built block %s with first opt kind %d\n",block->name.c_str(),block->opt->kind);
            }
            

            break;

        case STATEMENT_LIST:
            {
                Operation *tempOpt = nullptr;
                Operation *firstOpt = nullptr;
                int index = 0;
                ASTNode *temp = node;
                while(temp){
                    Operation *newOp = readVariablesWithNode(temp->ptr[0],list,index);
                    if (newOp)
                    {
                        if (firstOpt)
                        {
                            tempOpt -> next = newOp;
                            tempOpt = newOp;
                        }else{
                            firstOpt = newOp;
                            tempOpt = newOp;
                            printf("block %s first opt built with kind %d\n",block->name.c_str(),firstOpt -> kind);
                        }
                    }
                    
                    temp = temp->ptr[1];
                    index ++;
                }

                block -> opt = firstOpt;

            }

            if (block -> opt)
            {
                printf("built block %s with first opt kind %d\n",block->name.c_str(),block->opt->kind);
            }
            
            break;
        default:
            break;
        }
    }

}

void checkParamters(ASTNode *node,FunctionNode* func,VariableList *list,int level){
    ASTNode *temp = node;
    FuncCallOpt *funOpt = (FuncCallOpt *)currentOperation;
    for (int i = 0; i < func->parameters.size(); i++)
    {
        if (temp == nullptr)
        {
            printf("expecting arg count is %lud, but found null at %d,at line %d\n", func->parameters.size(),i+1,node -> pos);
            SemanticsError = true;
        }

        Parameter *param = func->parameters[i];
        ASTNode *arg = temp->ptr[0];

        if (arg == nullptr){
            printf("expecting arg count is %lud, but found null at %d,at line %d\n", func->parameters.size(),i+1,node -> pos);
            SemanticsError = true;
            return;
        }else{
            switch (arg->kind)
            {
            case ID:
                {
                    if (auto ret = search_variable_symbol(get<string>(arg->data),level,list)){
                        VarUseOpt *newOp = new VarUseOpt();
                        newOp->kind = ID;
                        newOp -> type = ret.value()->type;
                        newOp -> name = get<string>(arg->data);
                        if (param->type == ret.value()->type)
                        {
                            newOp -> return_type = newOp -> type;
                            funOpt -> args.push_back(newOp);
                        }else{
                            if (param -> type == Float && ret.value()->type == Int)
                            {
                                newOp -> return_type = Float;
                                funOpt -> args.push_back(newOp);
                            }else if (param -> type == Int && ret.value()->type == Char)
                            {
                                newOp -> return_type = Int;
                                funOpt -> args.push_back(newOp);
                            }else if (param -> type == Char && ret.value()->type == Int)
                            {
                                newOp -> return_type = Char;
                                funOpt -> args.push_back(newOp);
                            }else{
                                printf("unexpected arg type at %d,at line %d\n",i+1,node -> pos);
                                SemanticsError = true;
                                return;
                            }
                        }
        
                    }else{
                        printf("can't find this arg ID at %d,at line %d\n",i+1,node -> pos);
                        SemanticsError = true;
                    }
                }
                    
                break;
            case INTEGER:
                {
                    if (param->type == Int || param -> type == Float || param -> type == Void)
                    {
                        StaticValueOpt *newOp = new StaticValueOpt();
                        newOp->kind = INTEGER;
                        newOp -> return_type = Int;
                        newOp -> data = get<int>(node->data);
                        funOpt -> args.push_back(newOp);
                    }else{
                        printf("unexpected arg type at %d, expecting Int,at line %d\n",i+1,node -> pos);
                        SemanticsError = true;
                    }
                }
                break;
            case FLOAT:
                {
                    if (param->type == Float)
                    {
                        StaticValueOpt *newOp = new StaticValueOpt();
                        newOp->kind = FLOAT;
                        newOp -> return_type = Float;
                        newOp -> data = get<float>(node->data);
                        funOpt -> args.push_back(newOp);
                    }else{
                        printf("unexpected arg type at %d, expecting Char, at line %d\n",i+1,node -> pos);
                        SemanticsError = true;
                    }
                }
                break;
            case CHAR:
                {
                    if (param->type == Char || param -> type == Int)
                    {
                        StaticValueOpt *newOp = new StaticValueOpt();
                        newOp->kind = CHAR;
                        newOp -> return_type = Char;
                        newOp -> data = get<char>(node->data);
                        funOpt -> args.push_back(newOp);
                    }else{
                        printf("unexpected arg type at %d, expecting Float,at line %d\n",i+1,node -> pos);
                        SemanticsError = true;
                    }
                }
                break;
            default:
                printf("unexpected arg type at %d,at line %d\n",i+1,node -> pos);
                SemanticsError = true;
                break;
            }
            
        }
        
        temp = temp->ptr[1];
    }
    
}


void readVaribales(ASTNode *node){
    globalVars = new VariableList();
    globalVars->namespacing = "global";
    funcIndex = 0;
    readVariablesGlobal(node,0);
    if (SemanticsError)
    {
        printf("\nSyntax Error! Compile Process Quit early!\n");
        return;
    }
    
    print_llvm_ir(entryOperation);
    return;
}

optType getOptType(string raw){
    if (raw == "Int")
    {
        return Int;
    }else if (raw == "Char")
    {
        return Char;
    }else if (raw == "Float")
    {
        return Float;
    }else{
        return Void;
    }
    
}

compareType getCompareType(string raw){
    if (raw == "==")
    {
        return Equal;
    }else if (raw == "!=")
    {
        return NEqual;
    }else if (raw == ">")
    {
        return Large;
    }else if (raw == "<")
    {
        return Small;
    }else if (raw == ">=")
    {
        return LargeEqual;
    }else if (raw == "<=")
    {
        return SmallEqual;
    }

    return Equal;
}

variant<char,int,float> getStaticValue(variant<int,float,string,char> origin,optType type){
    try
    {
        if (type == Int)
        {
            int value = get<int>(origin);
            return value;
        }else if(type == Float){
            float value = get<float>(origin);
            return value;
        }else if(type == Char){
            char value = get<char>(origin);
            return value;
        }else{
            return 0;
        }
        
    }
    catch(std::bad_variant_access&)
    {
        return 0;
    }
    
}