#include "swift-lite.h"
#include "iostream"
#include "memory"
#include "list"
#include "unordered_set"

using std::pair, std::tuple, std::list, std::make_unique, std::unordered_set;

unsigned long int tempIndex = 0;

LLVMContext TheContext;
Module TheModule("app", TheContext);
vector<BasicBlock *> block_stack;
vector<IRBuilder<>> builder_stack;
vector<Function *> function_stack;

extern VariableList *globalVars;

unordered_map<string, pair<Function *, FunctionType *>> function_table;
unordered_map<string, pair<Value *, int>> val_table;
unordered_map<string, BasicBlock *> label_table;
unordered_set<BasicBlock *> finished_block;

typedef tuple<Instruction *, Value *, string, string> DeferredBrStatementType;

unordered_map<string, vector<Instruction *>> deferred_goto_statement;
list<DeferredBrStatementType> deferred_br_statement;
vector<GlobalVariable*> buildGVarDefine(Operation *opt);
vector<Value*> buildVarDefine(Operation *opt,BasicBlock *block);
Value* buildSimpleOpts(Operation *opt,VariableList *list,BasicBlock *block);
void buildInFuncOpts(Operation *opt,VariableList *list,BasicBlock *block);

ConstantInt* const_int_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantInt* const_char_val = ConstantInt::get(TheContext, APInt(32,0));
ConstantFP* const_float_val = ConstantFP::get(TheContext, APFloat(0.0));

// auto create_goto = [&](const string &label) -> void {
//     auto *current_basic_block = block_stack.back();
//     if (finished_block.count(current_basic_block))
//         return;
//     finished_block.insert(current_basic_block);

//     if (label_table.count(label))
//     {
//         builder_stack.back().CreateBr(label_table[label]);
//     }
//     else
//     {
//         Instruction *fake_node = builder_stack.back().CreateRetVoid();
//         deferred_goto_statement[label].emplace_back(fake_node);
//     }
// };



// extern vector<Symbol> symbol_table;

// pair<Value *, int> prepare_opn(LLVMContext &TheContext, unordered_map<string, pair<Value *, int>> &val_table, const Operation &operation)
// {
//     switch (operation.kind)
//     {
//     case INT:
//         return {ConstantInt::get(Type::getInt32Ty(TheContext), get<int>(operation.data)), INT};
//     case FLOAT:
//         return {ConstantFP::get(Type::getFloatTy(TheContext), get<float>(operation.data)), FLOAT};
//     case ID:
//         if (val_table.count(get<string>(operation.data)))
//             return val_table[get<string>(operation.data)];
//     }

//     return {nullptr, 0};
// }
void print_llvm_ir(Operation *head){
    builder_stack.emplace_back(IRBuilder<>(TheContext));

    Type *preAssignreturnType = Type::getVoidTy(TheContext);
    FunctionType *reAssignfunctionType = FunctionType::get(preAssignreturnType,false);
    Function *fpreAssignunctionValue = Function::Create(reAssignfunctionType,Function::ExternalLinkage,"preservedFunction_Global_var_assign_init",TheModule);
    BasicBlock *preAssignBlock = BasicBlock::Create(TheContext,"entry",fpreAssignunctionValue);
    IRBuilder<> preAssignBuilder(preAssignBlock);

    Operation *currentOpt = head;
    while (currentOpt)
    {
        switch (currentOpt -> kind)
        {
        case ASSIGN:
            {
                //must be var define assign
                
                NormalOpt *opt = (NormalOpt*) currentOpt;
                auto gvars = buildGVarDefine(opt -> left);
                builder_stack.push_back(preAssignBuilder);
                for (int i = 0; i < gvars.size(); i++)
                {
                    auto var = gvars[i]; //llvm
                    auto varable = search_variable_symbol_llvm(var->getName(),globalVars);//mine
                    varable -> llvmAI = var;
                    auto v = buildSimpleOpts(opt->right,globalVars,preAssignBlock);
                    preAssignBuilder.CreateStore(v,var);
                }
                builder_stack.pop_back();
                
            }
            break;
        case VAR_DEFINE:
            {
                auto gvars = buildGVarDefine(currentOpt);
                for (int i = 0; i < gvars.size(); i++)
                {
                    auto var = gvars[i]; //llvm
                    auto varable = search_variable_symbol_llvm(var->getName(),globalVars);//mine
                    varable -> llvmAI = var;
                }
            }
            break;
        case FUNCTION:
            {
                FuncAnnounceOpt *fOpt = (FuncAnnounceOpt *)currentOpt;
                Type *return_type;
                switch (fOpt -> func -> return_type)
                {
                case Void:
                    return_type = Type::getVoidTy(TheContext);
                    break;
                case Int: case Char:
                    return_type = Type::getInt32Ty(TheContext);
                    break;
                case Float:
                    return_type = Type::getFloatTy(TheContext);
                    break;
                default:
                    break;
                }

                vector<Type *> parameters;
                for (int i = 0; i < fOpt -> func -> parameters.size(); i++)
                {
                    auto arg = fOpt -> func -> parameters[i];
                    if (arg->type == Float)
                    {
                        parameters.push_back(Type::getFloatTy(TheContext));
                    }else{
                        parameters.push_back(Type::getInt32Ty(TheContext));
                    }

                }

                FunctionType *function_type = FunctionType::get(return_type, parameters, false);
                Function *function_value = Function::Create(function_type, Function::ExternalLinkage, fOpt -> func -> name, TheModule);
                BasicBlock *next_block = BasicBlock::Create(TheContext, "entry", function_value);
                function_table[fOpt -> func -> name] = {function_value, function_type};

                IRBuilder<> next_builder(next_block);
                builder_stack.push_back(next_builder);
                block_stack.push_back(next_block);
                function_stack.push_back(function_value);
                
                buildInFuncOpts(fOpt -> func -> block -> opt,fOpt -> func -> block -> varlist,next_block);

                
                
            }
            break;
        default:
            break;
        }
        currentOpt = currentOpt -> next;
    }

    preAssignBuilder.CreateRetVoid();
    
    if constexpr (PRINT_LLVM_IR == 1)
    {
        TheModule.print(errs(), nullptr);
    }
    verifyModule(TheModule, &(errs()));

    std::unique_ptr<Module> ptr(&TheModule);
    ExecutionEngine *engine = EngineBuilder(std::move(ptr)).create();
    engine->finalizeObject();
    engine->runFunction(function_table["main"].first, {});

}

vector<GlobalVariable*> buildGVarDefine(Operation *opt){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    vector<GlobalVariable*> vars;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_int_val,name);
                vars.push_back(newGVar);
            }
            break;
        case Char:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getInt32Ty(TheContext),false,GlobalVariable::ExternalLinkage,const_char_val,name);
                vars.push_back(newGVar);
            }
            break;
        case Float:
            {
                GlobalVariable *newGVar = new GlobalVariable(TheModule,Type::getFloatTy(TheContext),false,GlobalVariable::ExternalLinkage,const_float_val,name);
                vars.push_back(newGVar);
            }
            break;
        default:
            break;
        }
    }
    
    return vars;
}

vector<Value*> buildVarDefine(Operation *opt,BasicBlock *block){
    DefineOpt *defineOpt = (DefineOpt*)opt;
    vector<Value*> vars;
    for (int i = 0; i < defineOpt ->names.size(); i++)
    {
        string name = defineOpt -> names[i];
        switch (defineOpt ->type)
        {
        case Int:
            {
                auto AI = new AllocaInst(Type::getInt32Ty(TheContext),0,name,block);
                vars.push_back(AI);
            }
            
            break;
        case Char:
            {
                auto AI = new AllocaInst(Type::getInt32Ty(TheContext),0,name,block);
                vars.push_back(AI);
            }
            break;
        case Float:
            {
                auto AI = new AllocaInst(Type::getFloatTy(TheContext),0,name,block);
                vars.push_back(AI);
            }
            break;
        default:
            break;
        }
    }
    return vars;
}

Value* buildSimpleOpts(Operation *opt,VariableList *list,BasicBlock *block){
    
    switch (opt->kind)
    {
        case ID:
            {
                VarUseOpt *newOp = (VarUseOpt*) opt;
                auto var = search_variable_symbol_llvm(newOp->name,list);
                if (var -> llvmAI == nullptr)
                {
                    //then its parameters;
                    auto func = function_stack.back();
                    auto naiveFunc = search_function_symbol(func->getName());
                    auto params = naiveFunc.value()->parameters;
                    int index = 0;
                    for (int i = 0; i < params.size(); i++)
                    {
                        if (var -> name == params[i]->name)
                        {
                            index = i;
                        }
                        
                    }
                    
                    int temp = 0;
                    for (auto& A : func->args()) {
                        if (temp == index)
                        {
                            return &A;
                        }
                        
                        temp ++;
                    }
                    return nullptr;
                }else{
                    auto ai = var->llvmAI;
                    auto cl = builder_stack.back().CreateLoad(ai);
                    return cl;
                }
                
            }
            break;
        case INTEGER :case FLOAT: case CHAR:
            {
                StaticValueOpt *newOp = (StaticValueOpt*)opt;
                if (newOp -> kind == INTEGER || newOp -> kind == CHAR)
                {
                    if (newOp -> kind == INTEGER)
                    {
                        int value = get<int>(newOp -> data);
                        if (newOp -> return_type == Float){
                            auto v = ConstantFP::get(Type::getFloatTy(TheContext),(float)value);
                            return v;
                        }else{
                            auto v = ConstantInt::get(Type::getInt32Ty(TheContext), value);
                            return v;
                        }
                        
                    }else{
                        int value = (int)get<char>(newOp -> data);
                        auto v = ConstantInt::get(Type::getInt32Ty(TheContext), value);
                        return v;
                    }
                    
                }else{
                    float value = get<float>(newOp -> data);
                    auto v = ConstantFP::get(Type::getFloatTy(TheContext),value);
                    return v;
                }
                
            }
            break;
        case NOT: 
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto result = buildSimpleOpts(rOpt -> right,list,block);
                auto returnValue = builder_stack.back().CreateNot(result);
                if (rOpt -> right ->return_type == Float)
                {
                    auto compare = builder_stack.back().CreateFCmpOEQ(returnValue,ConstantFP::getNaN(Type::getFloatTy(TheContext)));
                    return compare;
                }else{
                    auto compare = builder_stack.back().CreateICmpEQ(returnValue,ConstantInt::get(Type::getInt32Ty(TheContext),0));
                    return compare;
                }
                
            }
            break;
        case INCREASE: 
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto l = buildSimpleOpts(rOpt -> right,list,block);
                auto r = ConstantInt::get(Type::getInt32Ty(TheContext), 1);
                auto returnValue = builder_stack.back().CreateAdd(l,r,l->getName());
                return returnValue;
            }
            break;
        case DECREASE:
            {
                RightOpt *rOpt = (RightOpt*)opt;
                auto l = buildSimpleOpts(rOpt -> right,list,block);
                auto r = ConstantInt::get(Type::getInt32Ty(TheContext), 1);
                auto returnValue = builder_stack.back().CreateSub(l,r,l->getName());
                return returnValue;
            }
            break;
        case COMPARE:
            {
                CompareOpt *cOpt = (CompareOpt *)opt;
                bool useFloat = false;
                if (cOpt -> left -> return_type == Float || cOpt -> right -> return_type == Float)
                {
                    useFloat = true;
                }
                switch (cOpt ->type)
                {
                case Equal:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            
                            auto result = builder_stack.back().CreateFCmpOEQ(l,r);
                            return result;
                        }else{
                            
                            auto result = builder_stack.back().CreateICmpEQ(l,r);
                            return result;
                        }
                    }
                    break;
                case NEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            
                            auto result = builder_stack.back().CreateFCmpONE(l,r);
                            return result;
                        }else{
                            
                            auto result = builder_stack.back().CreateICmpNE(l,r);
                            return result;
                        }
                    }
                    break;
                case Large:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOGT(l,r);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSGT(l,r);
                            return result;
                        }
                    }
                    break;
                case Small:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOLT(l,r);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSLT(l,r);
                            return result;
                        }
                    }
                    break;
                case LargeEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOGE(l,r);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSGE(l,r);
                            return result;
                        }
                    }
                    break;
                case SmallEqual:
                    {
                        auto l = buildSimpleOpts(cOpt -> left,list,block);
                        auto r = buildSimpleOpts(cOpt -> right,list,block);
                        
                        if (useFloat)
                        {
                            auto result = builder_stack.back().CreateFCmpOLE(l,r);
                            return result;
                        }else{
                            auto result = builder_stack.back().CreateICmpSLE(l,r);
                            return result;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        case AND: case OR: 
            {
                NormalOpt *nOpt = (NormalOpt *)opt;
                auto l = buildSimpleOpts(nOpt -> left,list,block);
                auto r = buildSimpleOpts(nOpt -> right,list,block);
                
                if (opt -> kind == AND)
                {
                    auto result = builder_stack.back().CreateAnd(l,r);
                    return result;
                }else{
                    auto result = builder_stack.back().CreateOr(l,r);
                    return result;
                }
                
            }
            break;
        case PLUS: 
        case MINUS: 
        case MULTI: 
        case DIVID:
            {
                NormalOpt *nOpt = (NormalOpt *)opt;
                auto l = buildSimpleOpts(nOpt -> left,list,block);
                auto r = buildSimpleOpts(nOpt -> right,list,block);
                bool useFloat = false;
                if (nOpt -> left -> return_type == Float || nOpt -> right -> return_type == Float)
                {
                    useFloat = true;
                }
                switch (opt -> kind)
                {
                case PLUS:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFAdd(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateAdd(l,r,l->getName());
                        return result;
                    }
                    break;
                case MINUS:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFSub(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateSub(l,r,l->getName());
                        return result;
                    }
                case MULTI:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFMul(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateMul(l,r,l->getName());
                        return result;
                    }
                case DIVID:
                    if (useFloat)
                    {
                        auto result = builder_stack.back().CreateFDiv(l,r,l->getName());
                        return result;
                    }else{
                        auto result = builder_stack.back().CreateSDiv(l,r,l->getName());
                        return result;
                    }
                    break;
                default:
                    break;
                }
            }

        case FUNC_CALL:
            {
                

                FuncCallOpt *fOpt = (FuncCallOpt*)opt;
                auto [fn, fnType] = function_table[fOpt -> func -> name];
                vector<Value*> params;
                for (int i = 0; i < fOpt->args.size(); i++)
                {
                    Operation *todoP;
                    auto arg = fOpt->args[i];
                    try {
                        VarUseOpt* p = get<VarUseOpt*>(arg);
                        todoP = p;
                        
                    }catch (std::bad_variant_access&) {
                        try {
                            StaticValueOpt* p = get<StaticValueOpt*>(arg);
                            
                            todoP = p;

                        }catch (std::bad_variant_access&) {
                            //do nothing
                        }
                    }

                    auto doneValue = buildSimpleOpts(todoP,list,block);
                    params.push_back(doneValue);

                }
                
                auto result = builder_stack.back().CreateCall(fn,params);
                return result;
            }
            
            break;
        default:
            return nullptr;
            break;
    }
    return nullptr;
}

void buildInFuncOpts(Operation *opt,VariableList *list,BasicBlock *block){

    Operation *currentOpt = opt;
    int debugV = 0;
    while (currentOpt){
            switch (currentOpt -> kind)
        {
            case AND: case OR: case NOT: case PLUS: case MINUS: case MULTI: case DIVID:
            case COMPARE: case INCREASE: case DECREASE:
            case INTEGER: case FLOAT: case CHAR: case ID: case FUNC_CALL:
                buildSimpleOpts(currentOpt,list,block);
                break;
            case ASSIGN:
                {
                    NormalOpt *nOpt = (NormalOpt*) currentOpt;
                    auto r = buildSimpleOpts(nOpt -> right,list,block);
                    if (nOpt -> left -> kind == VAR_DEFINE)
                    {
                        auto vars = buildVarDefine(nOpt -> left,block);
                        for (int i = 0; i < vars.size(); i++)
                        {
                            auto var = vars[i];
                            auto trueVar = search_variable_symbol_llvm(var->getName(),list);
                            trueVar ->llvmAI = var;
                            builder_stack.back().CreateStore(r,var);
                        }
                    }else{
                        auto l = buildSimpleOpts(nOpt -> left,list,block);

                        builder_stack.back().CreateStore(r,l);
                    }
                    
                }
                break;
            case VAR_DEFINE:
                {
                    auto result = buildVarDefine(currentOpt,block);
                    for (int i = 0; i < result.size(); i++)
                    {
                        auto var = result[i];
                        auto trueVar = search_variable_symbol_llvm(var->getName(),list);
                        trueVar ->llvmAI = var;
                    }
                }
                break;
            case RETURN:
                {
                    RightOpt *rOpt = (RightOpt *)currentOpt;
                    if (rOpt -> return_type == Void)
                    {
                        builder_stack.back().CreateRetVoid();
                    }else{
                        auto r = buildSimpleOpts(rOpt -> right,list,block);
                        builder_stack.back().CreateRet(r);
                    }

                }
                break;
            case CONTINUE: case BREAK:
                {
                    
                }
                break;
            case WHILE:
                {
                    WhileOpt *wOpt = (WhileOpt*)currentOpt;
                    BasicBlock *conditionBasicBlock = BasicBlock::Create(TheContext, "whileCondition", function_stack.back());
                    BasicBlock *ifBasicBlock = BasicBlock::Create(TheContext, "whileIf", function_stack.back());
                    
                    builder_stack.back().CreateBr(conditionBasicBlock);
                    
                    IRBuilder<> condBuilder(conditionBasicBlock);
                    builder_stack.push_back(condBuilder);
                    block_stack.push_back(conditionBasicBlock);
                    auto condition = buildSimpleOpts(wOpt->condintion->condition,list,conditionBasicBlock);
                    
                    IRBuilder<> ifBuilder(ifBasicBlock);
                    builder_stack.push_back(ifBuilder);
                    block_stack.push_back(ifBasicBlock);
                    buildInFuncOpts(wOpt->ifBlock->opt,wOpt->ifBlock->varlist,ifBasicBlock);
                    builder_stack.back().CreateBr(conditionBasicBlock);
                    
                    BasicBlock *afterBasicBlock = BasicBlock::Create(TheContext, "afterWhile", function_stack.back());
                    IRBuilder<> afBuilder(afterBasicBlock);
                    builder_stack.push_back(afBuilder);
                    block_stack.push_back(afterBasicBlock);

                    condBuilder.CreateCondBr(condition,ifBasicBlock,afterBasicBlock);

                }
                break;
            case IF_THEN: case IF_THEN_ELSE:
                {
                    IfOpt *iOpt = (IfOpt*)currentOpt;
                    BasicBlock *ifBasicBlock = BasicBlock::Create(TheContext, "if", function_stack.back());
                    BasicBlock *elseBasicBlock = BasicBlock::Create(TheContext, "else", function_stack.back());
                    BasicBlock *afterBasicBlock = BasicBlock::Create(TheContext, "afterIf", function_stack.back());
                    
                    auto condition = buildSimpleOpts(iOpt->condintion->condition,list,block);

                    builder_stack.back().CreateCondBr(condition,ifBasicBlock,elseBasicBlock);

                    IRBuilder<> ifBuilder(ifBasicBlock);
                    builder_stack.push_back(ifBuilder);
                    block_stack.push_back(ifBasicBlock);
                    buildInFuncOpts(iOpt->ifBlock->opt,iOpt->ifBlock->varlist,ifBasicBlock);
                    builder_stack.back().CreateBr(afterBasicBlock);
                    

                    IRBuilder<> elseBuilder(elseBasicBlock);
                    builder_stack.push_back(elseBuilder);
                    block_stack.push_back(elseBasicBlock);

                    if (currentOpt -> kind == IF_THEN_ELSE)
                    {
                        buildInFuncOpts(iOpt->elseBlock->opt,iOpt->elseBlock->varlist,elseBasicBlock);
                    }
                    builder_stack.back().CreateBr(afterBasicBlock);
                    
                    IRBuilder<> afBuilder(afterBasicBlock);
                    builder_stack.push_back(afBuilder);
                    block_stack.push_back(afterBasicBlock);

                }
                break;
            default:
                break;
        }

        currentOpt = currentOpt -> next;
        debugV ++;
        printf("ir function progress %s %d\n",list->namespacing.c_str(),debugV);
    }
    

}
// variant<char,int,float> 